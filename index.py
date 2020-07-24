import os
import psycopg2
from psycopg2.extras import RealDictCursor


def WritResult(res , filename):
    file = open("result/"+filename+".txt", "w")
    for row in res:
        for key in row:
            file.write(key+":"+str(row[key])+"\n")

        file.write("===============\n")

    file.close()


if not os.path.isdir('result'):
    os.makedirs("result")

connection = psycopg2.connect(user="star", password="Amirhossein", host="127.0.0.1", port="5432", database="fpdb")
try:
    cursor = connection.cursor(cursor_factory=RealDictCursor)
    cursor.execute("select * from fp_stores_data where has_sold = (select max(has_sold) from fp_stores_data);")
    result = cursor.fetchall()
    WritResult(result, "max_sold_market")

    cursor.execute("select * from fp_city_aggregation where has_sold = (select max(has_sold) from fp_city_aggregation)")
    result = cursor.fetchall()
    WritResult(result, "max_sold_city")

    cursor.execute("select * from fp_city_aggregation where price = (select max(price) from fp_city_aggregation)")
    result = cursor.fetchall()
    WritResult(result, "max_average_price_in city")

    cursor.execute("select * from fp_store_aggregation where price = (select max(price) from fp_store_aggregation)")
    result = cursor.fetchall()
    WritResult(result, "max_SUM_OF_PRICE in market")

    cursor.execute("select * from fp_stores_data where  quantity = (select min(quantity) from fp_stores_data where has_sold = (select max(has_sold) from fp_stores_data)) order by has_sold desc ")
    result = cursor.fetchall()
    WritResult(result, "The market with max sold and min quantity in market")

    # cursor.execute("select product_id,count(product_id) from fp_stores_data where product_id > 0 group by product_id")
    # result = cursor.fetchall()
    # WritResult(result, "count of so in market")

except (Exception, psycopg2.Error) as error:
    print("Error while connecting to PostgreSQL", error)
finally:
    if connection:
        cursor.close()
        connection.close()
        print("PostgresQL connection is closed")
