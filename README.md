#Project


to start i use CentOs as OS and if you should linux you should install postgrsql and gcc and python

and at first i should write bash script that download file from url ....

and then install postgrsql ...

to install postgresql in centOs 7 :

===================================================
sudo yum install postgresql-server postgresql-contrib

sudo postgresql-setup initdb

sudo systemctl start postgresql

sudo systemctl enable postgresql

sudo -u postgres psql postgres

before all we should start postgresql service:
----->sudo systemctl start postgresql
to enable it for auto start:
----->sudo systemctl enable postgresql

create user myusername with password 'mypassword'
// my userName is star and my password is Amirhossein@0022970916
create database dbname owner username
//create database fpdb owner star
grant previlage on database dbname to username
//grant all on database fpdb to star
after doing all of this we should add user to pg_hba.conf
for this we sould go to postgres shell 
	---------------> su - postgres
	---------------> cd data
	---------------> vim pg_hba.conf

and then we should add own user and database to list with md5 method
after all of this we should restart our postgresql service and then it's ready to use
	systemctl restart postgresql

-->creating tables 
su - postgres
psql fpdb star -W
-->then enter your password
--->now we can type postgres command

CREATE TABLE IF NOT EXISTS fp_stores_data (
  id SERIAL PRIMARY KEY,
  time BIGINT,
  province VARCHAR(50) NOT NULL,
  city VARCHAR(50) NOT NULL,
  market_id INTEGER NOT NULL,
  product_id INTEGER NOT NULL,
  price INTEGER NOT NULL,
  quantity INTEGER NOT NULL,
  has_sold INTEGER NOT NULL
);

create table fp_city_aggregation
(
    id       serial not null constraint fp_city_aggregation_pkey primary key,
    time     bigint,
    province varchar(50) not null,
    city     varchar(50) not null,
    price    integer     not null,
    quantity integer     not null,
    has_sold integer     not null
);

====================================================
to compile every c program in linux by gcc we use this command to make a 
executable file and then run it:
--------------------->
gcc file.c -o executable
./executable
<---------------------

after install all dependensity of c and library
----------------------> yum install postgresql-devel
to" add library of postgresql i use C99 for gnu c compiler 
we need to add "-lpq -std=c99" option

to download and extract input file we use bash script.
also we can wite own bash script in crontab
we use cronjob to schedule for 1 min
to wite a corn tab we should use this command for each user we want: crontab -e
after we can use an editor and write in file

*/1 * * * * wget -P /tmp/final_project loh.istgahesalavati.ir/report.gz.tar
*/1 * * * * tar -xvzf /tmp/final_project/report.gz.tar --directory=/tmp/final_project/
*/1 * * * * rm -rf /tmp/final_project/report.gz.tar


then save it ....if this not work please check status of service cron
*/1 means that command CMD run every 1min now we can change it to daily or monthly at fix time by changing specefic * symbole

before that i use wget that you can install it from pkg-manager
and i download http://loh.istgahesalavati.ir/report.gz.tar to /tmp/final_project/
and also at first time i create a shortcut or link with name data with this command 
----IN THE PROJECT LOCATION------->  ln -s /tmp/final_project data
and every time that i call getUpdate.bash ,we resive a new report.gz.tar and extract it on same folder then delete .gz.tar file

---------------------------------------------
today i start to write index.c that contain psql lib

that's a library that easy to use just shuld create a connection and exceute command at this
and then we should close connection

at start i Read File one by one from data directory ...
for this work(walk at data dir) , we can use <dirnet.h>

we also can create some struct to easier work with data

at first i give data from file LINE by LINE then put it to database
then remove this file and read next file...
