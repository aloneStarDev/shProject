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

struct Data{
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
struct Data * fData = NULL;
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
		d->avgPrice/=d->quantity;
		sprintf(cmd,"Insert into fp_city_aggregation (time,province,city,price,quantity,has_sold)values(%ld,'%s','%s',%ld,%d,%d)",d->time,d->province,d->city,d->avgPrice,d->quantity,d->has_sold);
		PGresult * rs = PQexec(conn,cmd);
		free(cmd);
		if (PQresultStatus(rs) != PGRES_COMMAND_OK){
			printf("an error accurre:%s",PQerrorMessage(conn));
			closeConn(conn); 
			return;
		}else
			printf("inserted City Aggregation\n");
	}
	closeConn(conn);
}
void City_Aggregation(){
	struct CityAggregation * ca = NULL;
	for(struct Data * d = fData;d!=NULL;d=d->next)
	{
		if(ca == NULL){
			ca = (struct CityAggregation *)malloc(sizeof(struct CityAggregation));
			ca->time =d->time;
			strcpy(ca->city,d->city);
			strcpy(ca->province,d->province);
			ca->avgPrice = d->price;
			ca->quantity = d->quantity;
			ca->has_sold = d->has_sold;
			ca->count=1;
			ca->next = NULL;
		}
		int find = 0;
		for(struct CityAggregation * cad = ca;cad!=NULL;cad=cad->next){
			if(strcmp(d->city,cad->city)==0){
				find = 1;
				cad->count++;
				cad->avgPrice+=d->price;
				cad->has_sold+=d->has_sold;
				cad->quantity+=d->quantity;
			}
		}
		if(!find){
			struct CityAggregation * tmp; 
			for(tmp = ca;tmp->next!=NULL;tmp=tmp->next);
			tmp->next = (struct CityAggregation *)malloc(sizeof(struct CityAggregation));
			tmp->next->time =d->time;
			strcpy(tmp->next->city,d->city);
			strcpy(tmp->next->province,d->province);
			tmp->next->avgPrice = d->price;
			tmp->next->quantity = d->quantity;
			tmp->next->has_sold = d->has_sold;
			tmp->next->count=1;
			tmp->next->next = NULL;
		}
	}
	InsertCityAggregation(ca);
	free_cityAggregation(ca);
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
		}else
			printf("inserted Store Aggregation\n");
	}
	closeConn(conn);
}
void Store_Aggregation(){
	struct StoreAggregation * sa = NULL;
	for(struct Data * d = fData;d!=NULL;d=d->next)
	{
		if(sa == NULL){
			sa = (struct StoreAggregation *)malloc(sizeof(struct StoreAggregation));
			sa->market_id =d->market_id;
			strcpy(sa->city,d->city);
			sa->price = d->price;
			sa->quantity = d->quantity;
			sa->has_sold = d->has_sold;
			sa->count=1;
			sa->next = NULL;
		}
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
			struct StoreAggregation * tmp; 
			for(tmp = sa;tmp->next!=NULL;tmp=tmp->next);
			tmp->next = (struct StoreAggregation *)malloc(sizeof(struct StoreAggregation));
			tmp->next->market_id = d->market_id;
			strcpy(tmp->next->city,d->city);
			tmp->next->price = d->price;
			tmp->next->quantity = d->quantity;
			tmp->next->has_sold = d->has_sold;
			tmp->next->count=1;
			tmp->next->next = NULL;
		}
	}
	InsertStoreAggregation(sa);
	free_storeAggregation(sa);
}

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
				// int len = strlen(ent->d_name);
				// if(len > 5 && strcmp(ent->d_name + len - 5, ".text") == 0)
				// {
					closedir (dir);
					return ent->d_name;
				// }			
			}
		}
		closedir(dir);
		return NULL;
	} else {
	/* could not open directory */
	perror ("could not open directory"); 
	
	}
}
struct Data *  fetch_data(char * address,struct Data * last)
{
	conn = PQconnectdb("user=star password=Amirhossein dbname=fpdb");
	if(PQstatus(conn) == CONNECTION_BAD){
		fprintf(stderr,"Connection faild : %s\n",PQerrorMessage(conn));
		closeConn(conn);
	}
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
	
	City_Aggregation();
	// Store_Aggregation();
	free_data();
	printf("\nfinished\n");
	free(file);
	return 0;
}
