*/1 * * * * wget -P /tmp/final_project loh.istgahesalavati.ir/report.gz.tar
*/1 * * * * tar -xvzf /tmp/final_project/report.gz.tar --directory=/tmp/final_project/
*/1 * * * * rm -rf /tmp/final_project/report.gz.tar
