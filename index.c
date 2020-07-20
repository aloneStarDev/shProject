#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>
#include <string.h>
#include <dirent.h>

void closeConn(PGconn * conn){	
	PQfinish(conn);
	exit(1);	
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
	FILE * fp = fopen(address,"a+");
	while(1){
		char line[200];
		fgets(line,200,fp);
		if(feof(fp))
			break;
		printf("%s",line);
	}
	fclose(fp);
}
int main() {
 /*   
	PGconn * conn = PQconnectdb("user=star password=Amirhossein dbname=fpdb");
	if(PQstatus(conn) == CONNECTION_BAD){
		fprintf(stderr,"Connection faild : %s\n",PQerrorMessage(conn));
		closeConn(conn);
	}
	char text[] = "Insert into fp_stores_data values(";
	FILE * fp = fopen("","a+");
	fclose(fp);
	char * cmd = strcat(text,line);
	char * cmd = strcat(cmd,")");
	PGresult * rs = PGexec(conn,cmd);
    	
*/
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
		file = printfiles();
	}
	return 0;
}
