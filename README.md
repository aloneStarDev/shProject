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
  province VARCHAR(50) NOT NULL
  city VARCHAR(50) NOT NULL,
  market_id INTEGER NOT NULL,
  produt_id INEGER NOT NULL,
  price INTEGER NOT NULL,
  quantity INTEGER NOT NULL,
  has_sold INTEGER NOT NULL
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
so now i write this bash file

before that i use wget that you can install it from pkg-manager
and i download http://loh.istgahesalavati.ir/report.gz.tar to /tmp/final_project/
and also at first time i create a shortcut or link with name data with this command 
----IN THE PROJECT LOCATION------->  ln -s /tmp/final_project data
and every time that i call getUpdate.bash ,we resive a new report.gz.tar and extract it on same folder then delete .gz.tar file

---------------------------------------------
today i start to write index.c that contain psql lib


