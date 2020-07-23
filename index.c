#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>
#include <string.h>
#include <dirent.h>
#include<errno.h>

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
};
struct CityAggregation{
	long time;
	char province[50];
	char city[50];
	long avgPrice;
	int quantity;
	int has_sold;
};

void manage_city_aggregation(){
	
}

char * printfiles(){
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir ("data")) != NULL) {
	/* print all the files and directories within directory */
	while ((ent = readdir (dir)) != NULL) {
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
	return NULL;
	} else {
	/* could not open directory */
	perror (""); 
	
	}
}
void fetch_data(char * address)
{
	PGconn * conn = PQconnectdb("user=star password=Amirhossein dbname=fpdb");
	if(PQstatus(conn) == CONNECTION_BAD){
		fprintf(stderr,"Connection faild : %s\n",PQerrorMessage(conn));
		closeConn(conn);
	}
	int counter =0;
	FILE * fp = fopen(address,"a+");
	struct CityAggregation * ca = NULL;
	while(!feof(fp)){
		char *line =(char *) malloc(200);
		fflush(fp);
		fgets(line,200,fp);
		// printf("%s\n",line);
		if(feof(fp))
		{
			printf("test1\n");
			if(ca !=NULL)
				free(ca);
			closeConn(conn);
			break;
		}
		struct Data * d = (struct Data *) malloc(sizeof(struct Data));
		char * item = NULL;
		item = strtok(line,",");
		d->time = strtol(item,NULL,10);
		item = strtok(NULL,",");
		strcpy(d->province,item);
		item = strtok(NULL,",");
		strcpy(d->city,item);
		// printf("%d\n",counter);
		if(ca !=NULL && strcmp(ca->city,d->city)==0)
			counter++;
		else if (counter != 0){
			// printf("%s ===========> %d\n",ca->city,counter);
			char * cmdCityAggregation = malloc(300);
			ca->avgPrice/=ca->quantity;
			sprintf(cmdCityAggregation,"insert into fp_city_aggregation (time,province,city,price,quantity,has_sold) values(%ld,'%s','%s',%ld,%d,%d)",ca->time,ca->province,ca->city,ca->avgPrice,ca->quantity,ca->has_sold);
			PGresult * rs = PQexec(conn,cmdCityAggregation);
			// printf("%s\n",cmdCityAggregation);
			free(cmdCityAggregation);
			if (PQresultStatus(rs) != PGRES_COMMAND_OK){
				printf("an error accurre:%s",PQerrorMessage(conn));
				closeConn(conn);
			}
			PQclear(rs);
			free(ca);
			ca = (struct CityAggregation *) malloc(sizeof(struct CityAggregation *));
			counter = 1;
			ca->has_sold=0;
			ca->quantity=0;
			ca->avgPrice=0;
			ca->time=d->time;
			strcpy(ca->city,d->city);
			strcpy(ca->province,d->province);
		}else{
			ca = (struct CityAggregation *) malloc(sizeof(struct CityAggregation *));
			ca->has_sold=0;
			ca->quantity=0;
			ca->avgPrice=0;
			ca->time=d->time;
			strcpy(ca->city,d->city);
			strcpy(ca->province,d->province);
			counter++;
		}
		item = strtok(NULL,",");
		d->market_id = (int)strtol(item,NULL,10);
		item = strtok(NULL,",");
		d->product_id = (int)strtol(item,NULL,10);
		item = strtok(NULL,",");
		d->price = (int)strtol(item,NULL,10);
		if(strcmp(ca->city,d->city)==0)
			ca->avgPrice+=d->price;
		item = strtok(NULL,",");
		d->quantity = (int)strtol(item,NULL,10);
		if(strcmp(ca->city,d->city)==0)
			ca->quantity +=d->quantity;
		item = strtok(NULL,",");
		d->has_sold = (int)strtol(item,NULL,10);
		if(strcmp(ca->city,d->city)==0)
			ca->has_sold +=d->has_sold;
		char * cmd = malloc(300);
		sprintf(cmd,"Insert into fp_stores_data (time,province,city,market_id,product_id,price,quantity,has_sold)values(%ld,'%s','%s',%d,%d,%ld,%d,%d)",d->time,d->province,d->city,d->market_id,d->product_id,d->price,d->quantity,d->has_sold);
		free(d);
		PGresult * rs = PQexec(conn,cmd);
		free(cmd);
	    if (PQresultStatus(rs) != PGRES_COMMAND_OK){
			printf("an error accurre:%s",PQerrorMessage(conn));
			PQclear(rs);
     		closeConn(conn); 
		}
		// printf("%d\n",counter);
		PQclear(rs);
		free(line);
		
	}
	// free(ca);
	fclose(fp);
}
int main() {
	char * file = printfiles();
	char address[30] = "data";
	address[4]='/';
	while(file!=NULL)
	{
		int l = strlen(file);
		for(int i=0;i<l;i++)
			address[i+5]=file[i];
		address[l+5]='\0';
		fetch_data(address);
		if(!remove(address)){
			printf("deleted==============Next File============================\n");
		}
		file = printfiles();
	}
	return 0;
}
