#include<stdio.h>
#include<stdlib.h>
#include<libpq-fe.h>
#include<string.h>
#include<dirent.h>
#include<errno.h>


PGconn * conn;
void closeConn(PGconn * conn){	
	PQfinish(conn);
}

struct Data{//we store row data in link list 
	long time;
	char province[50];
	char city[50];
	int market_id;
	int product_id;
	long price;
	int quantity;
	int has_sold;
	struct Data * next;
} ;
struct Data * fData = NULL;//pointer----> head of raw Data
struct Data * AddRowData(
	struct Data * last,
	long time ,
	char province[50],
	char city[50],
	int market_id,
	int product_id,
	long price,
	int quantity,
	int has_sold
	){
		struct Data * data = (struct Data *) malloc(sizeof(struct Data));
		data->next = NULL;
		data->time = time;
		strcpy(data->province,province);
		strcpy(data->city,city);
		data->market_id = market_id;
		data->product_id = product_id;
		data->price = price;
		data->quantity = quantity;
		data->has_sold = has_sold;
		/* at first time at main last == first so we should  allocate a Node for head of linked list */
		if(last == NULL)
			fData = data;
		else
			last->next = data;
		last = data;
		struct Data * d = data;
		char * cmd = malloc(300);
		sprintf(cmd,"Insert into fp_stores_data (time,province,city,market_id,product_id,price,quantity,has_sold)values(%ld,'%s','%s',%d,%d,%ld,%d,%d)",d->time,d->province,d->city,d->market_id,d->product_id,d->price,d->quantity,d->has_sold);
		PGresult * rs = PQexec(conn,cmd);
		free(cmd);
	    if (PQresultStatus(rs) != PGRES_COMMAND_OK){// error
			printf("an error accurre:%s",PQerrorMessage(conn));
     		closeConn(conn); 
		}else{
			// inserted
		}
		PQclear(rs);
		return last;
	}

struct CityAggregation{
	long time;
	char province[50];
	char city[50];
	long avgPrice;
	int quantity;
	int has_sold;
	int count;
	struct CityAggregation * next;
};
void free_cityAggregation(struct CityAggregation * start){
	while(start!=NULL){
		struct CityAggregation * temp = start->next;
		free(start);
		start = temp;
	}
}
void InsertCityAggregation(struct CityAggregation * start){
	conn = PQconnectdb("user=star password=Amirhossein dbname=fpdb");
	for(struct CityAggregation * d = start;d!=NULL;d=d->next){
		char * cmd = malloc(300);
		d->avgPrice/=d->has_sold;//we also update each avrage before insert to database
		sprintf(cmd,"Insert into fp_city_aggregation (time,province,city,price,quantity,has_sold)values(%ld,'%s','%s',%ld,%d,%d)",d->time,d->province,d->city,d->avgPrice,d->quantity,d->has_sold);
		PGresult * rs = PQexec(conn,cmd);
		free(cmd);
		if (PQresultStatus(rs) != PGRES_COMMAND_OK){
			printf("an error accurre:%s",PQerrorMessage(conn));
			closeConn(conn); 
			return;
		}
		//else -> inserted City Aggregation
	}
	printf("City Aggregation finsish\n");
	closeConn(conn);
}
/* to find count of all field in each city we also store every city in on CityAggregation Node */
void City_Aggregation(){
	struct CityAggregation * ca = NULL;//create head of link list;
	for(struct Data * d = fData;d!=NULL;d=d->next)
	{
		int find = 0;
		/*search in linked list to find if this data also has any previouse value at link list*/
		for(struct CityAggregation * cad = ca;cad!=NULL;cad=cad->next){
			/*if we can find any samilar city to this node of data link list then we can update it at CityAggregation linked list*/
			if(strcmp(d->city,cad->city)==0){
				find = 1;
				cad->count++;
				cad->avgPrice+=d->price;// i also add it to other them but when i want to store it to database i change Sum to Avrage
				cad->has_sold+=d->has_sold;
				cad->quantity+=d->quantity;
			}
		}
		if(!find){
			/*if we cant find this city in CityAggregation we should add it to list*/
			struct CityAggregation * node;
			node = (struct CityAggregation *)malloc(sizeof(struct CityAggregation));
			node->time =d->time;
			strcpy(node->city,d->city);
			strcpy(node->province,d->province);
			node->avgPrice = d->price;
			node->quantity = d->quantity;
			node->has_sold = d->has_sold;
			node->count=1;
			node->next = NULL;
			/* at first time ca is NULL */
			if(ca==NULL)
				ca = node;
			else{
				/*we walk along City Aggregation linked list to achive lastNode then we can add this new data to end of this chain*/
				struct CityAggregation * tmp;
				for(tmp = ca;tmp->next!=NULL;tmp=tmp->next);
				tmp->next = node;
			}
		}
	}
	InsertCityAggregation(ca);//after creating a full City aggregation of all data we can insert it to database
	free_cityAggregation(ca);//then free this City Aggregation LinkList
}
void free_data(){
	while(fData!=NULL){
		struct Data * temp = fData->next;
		free(fData);
		fData = temp;
	}
}
struct StoreAggregation
{
    int market_id;
    int quantity;
    int has_sold;
    long price;
	char city[50];
	int count;
	struct StoreAggregation * next;
};
void free_storeAggregation(struct StoreAggregation* start){
	while(start!=NULL){
		struct StoreAggregation * temp = start->next;
		free(start);
		start = temp;
	}
}
void InsertStoreAggregation(struct StoreAggregation * start){
	conn = PQconnectdb("user=star password=Amirhossein dbname=fpdb");
	for(struct StoreAggregation * d = start;d!=NULL;d=d->next){
		char * cmd = malloc(300);
		sprintf(cmd,"Insert into fp_store_aggregation (market_id,city,price,quantity,has_sold)values(%d,'%s',%ld,%d,%d)",d->market_id,d->city,d->price,d->quantity,d->has_sold);
		PGresult * rs = PQexec(conn,cmd);
		free(cmd);
		if (PQresultStatus(rs) != PGRES_COMMAND_OK){
			printf("an error accurre:%s",PQerrorMessage(conn));
			closeConn(conn); 
			return;
		}
		//else -> "inserted Store Aggregation
	}
	printf("Store Aggregation finsish\n");
	closeConn(conn);
}
void Store_Aggregation(){
	struct StoreAggregation * sa = NULL;
	for(struct Data * d = fData;d!=NULL;d=d->next)
	{
		int find = 0;
		for(struct StoreAggregation * sad = sa;sad!=NULL;sad=sad->next){
			if(d->market_id==sad->market_id){
				find = 1;
				sad->count++;
				sad->price +=d->price;
				sad->has_sold+=d->has_sold;
				sad->quantity+=d->quantity;
			}
		}
		if(!find){
			struct StoreAggregation * node; 
			node = (struct StoreAggregation *)malloc(sizeof(struct StoreAggregation));
			node->market_id = d->market_id;
			strcpy(node->city,d->city);
			node->price = d->price;
			node->quantity = d->quantity;
			node->has_sold = d->has_sold;
			node->count=1;
			node->next = NULL;
			if(sa==NULL)
				sa = node;
			else{
				struct StoreAggregation * tmp;
				for(tmp = sa;tmp->next!=NULL;tmp=tmp->next);
				tmp->next = node;
			}
		}
	}
	printf("%d\n",sa->count);
	InsertStoreAggregation(sa);
	free_storeAggregation(sa);
}
/*get file name*/
char * printfiles(){
	DIR * dir;
	struct dirent * ent;
	if ((dir = opendir ("data")) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir (dir)) != NULL) {
			if(ent == NULL)
				break;
			printf("-------->%s\n",ent->d_name);
			if(strcmp(ent->d_name,".")==0 || strcmp(ent->d_name,"..")==0)
				continue;
			else{
				closedir (dir);
				return ent->d_name;
			}
		}
		closedir(dir);
		return NULL;
	} else {
	/* could not open directory */
	perror ("could not open directory"); 
	
	}
}
/* read data of file */
struct Data *  fetch_data(char * address,struct Data * last)
{
	/* create a connection */
	conn = PQconnectdb("user=star password=Amirhossein dbname=fpdb");
	if(PQstatus(conn) == CONNECTION_BAD){
		fprintf(stderr,"Connection faild : %s\n",PQerrorMessage(conn));
		closeConn(conn);
	}
	/* read data form file*/
	FILE * fp = fopen(address,"a+");


	while(!feof(fp)){
		char *line =(char *) malloc(200);
		fflush(fp);
		fgets(line,200,fp);
		if(feof(fp))
		{
			closeConn(conn);
			break;
		}
		
		//Data
		long time;
		char province[50];
		char city[50];
		int market_id;
		int product_id;
		long price;
		int quantity;
		int has_sold;

		char * item = NULL;
		item = strtok(line,",");
		time = strtol(item,NULL,10);
		
		item = strtok(NULL,",");
		strcpy(province,item);

		item = strtok(NULL,",");
		strcpy(city,item);
		
		item = strtok(NULL,",");
		market_id = (int)strtol(item,NULL,10);

		item = strtok(NULL,",");
		product_id = (int)strtol(item,NULL,10);

		item = strtok(NULL,",");
		price = (int)strtol(item,NULL,10);

		item = strtok(NULL,",");
		quantity = (int)strtol(item,NULL,10);

		item = strtok(NULL,",");
		has_sold = (int)strtol(item,NULL,10);
		
		/*write data in linked list and database*/
		last = AddRowData(last,time,province,city,market_id,product_id,price,quantity,has_sold);
		free(line);
		
	}
	fclose(fp);
	return last;
}
int main() {
	struct Data * lData = NULL;
	fData = lData;
	char * file = printfiles();
	char address[30] = "data";
	address[4]='/';
	while(file!=NULL)
	{
		int l = strlen(file);
		for(int i=0;i<l;i++)
			address[i+5]=file[i];
		address[l+5]='\0';
		lData = fetch_data(address,lData);
		if(!remove(address)){
			printf("======> %s \n------> finished & deleted\n",address);
		}
		file = printfiles();
	}
	/*
	* after read all data from file and then store it in fp_store_data
	* we can start aggregation ....for this i store every line of file in struct Data
	* and then each file that save in database then also add to linkedList
	*
	*
	*
	* we can use row data of linked list to aggregate City and Store
	* and then free each node of Data linked list
	*/
	City_Aggregation();//in this function i describe all but ,dont reapet it for store Aggregation 
	Store_Aggregation();//because it's very semilar to CityAggregation
	free_data();
	printf("\nfinished\n");
	free(file);
	return 0;
}
