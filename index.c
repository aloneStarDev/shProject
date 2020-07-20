#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>

int main() {
    
	PGconn * conn = PQconnectdb("user=star password=Amirhossein dbname=fpdb");
	if(PQstatus(conn) == CONNECTION_BAD){
	fprintf(stderr,"Connection faild : %s\n",PQerrorMessage(conn));
	PQfinish(conn);
	exit(1);	
	}
	int ser_ver = PQserverVersion(conn);
	printf("Version of server: %d\n", ser_ver);
    	
	PQfinish(conn);

    return 0;
}
