#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>
#include <string.h>
#include <dirent.h>

void closeConn(PGconn * conn){	
	PQfinish(conn);
	exit(1);	
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
char * printfiles(){
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir ("data")) != NULL) {
	/* print all the files and directories within directory */
	while ((ent = readdir (dir)) != NULL) {
		if(strcmp(ent->d_name,".")==0 || strcmp(ent->d_name,"..")==0)
			continue;
		else{
			closedir (dir);
			return ent->d_name;
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
	while(1){
		char line[200];
		fgets(line,200,fp);
		if(line[0]<'0' || line[0]>'9'){
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
		item = strtok(NULL,",");
		d->market_id = (int)strtol(item,NULL,10);
		item = strtok(NULL,",");
		d->product_id = (int)strtol(item,NULL,10);
		item = strtok(NULL,",");
		d->price = (int)strtol(item,NULL,10);
		item = strtok(NULL,",");
		d->quantity = (int)strtol(item,NULL,10);
		item = strtok(NULL,",");
		d->has_sold = (int)strtol(item,NULL,10);
		char text[] = "Insert into fp_stores_data (time,province,city,market_id,product_id,price,quantity,has_sold)values(";
		char * t = malloc(200);
		fflush(stdout);
		sprintf(t,"%ld,'%s','%s',%d,%d,%ld,%d,%d",d->time,d->province,d->city,d->market_id,d->product_id,d->price,d->quantity,d->has_sold);
		char * cmd = strcat(text,t);
		free(d);
		free(t);
		cmd = strcat(cmd,")");
		// printf("%s\n",cmd);
		PGresult * rs = PQexec(conn,cmd);
	    if (PQresultStatus(rs) != PGRES_COMMAND_OK){
			printf("an error accurre:%s",PQerrorMessage(conn));
     		closeConn(conn); 
		}
		counter++;
		// printf("%d\n",counter);
		PQclear(rs);
		if(feof(fp))
			break;
	}
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
		if(remove(address)){
			printf("err to delete\n");
			break;		
		}
		// else
		// {
			// printf("deleted=================");
		// }
		
		file = printfiles();
	}
	return 0;
}
