#include <my_global.h>
#include <mysql.h>

#include <string>
#include <iostream>
#include <list>
using namespace std;

int  finish_with_error(MYSQL *con)
{
  fprintf(stderr, "%s\n", mysql_error(con));
  mysql_close(con);
  return -1;     
}



struct connect_info
{
	string server;
	string user;
	string pwd;
	string db;
	connect_info():server("192.168.56.1"),user("root"),pwd("root"), db(defaultDB)
	{
		
	}
	static const char* defaultDB;
};

const char* connect_info::defaultDB = "testdb";

class Mysql
{
	MYSQL* _con;
	Mysql():_connected(false)
	{
		_con = mysql_init(0);		
	}
    bool _connected;
public:
	~Mysql()
	{
		cout<< "database resources released!" <<endl;
	}    
	MYSQL* getMysql()
	{
		if(!_connected)
			return 0;
		return _con;
	}
	int init(const connect_info& ci)
	{
		if (mysql_real_connect(_con, ci.server.c_str(), 
		ci.user.c_str(),ci.pwd.c_str(), 
          ci.db.c_str(), 0, 0, 0) == 0) 
		{
			_connected = false;
		}  
		else
		{
			_connected = true;
		}
	}
	static Mysql& instance()
	{
		static Mysql sql;
		return sql;
	} 
	static void destroy()
	{
		if(instance()._connected)
		{
			mysql_close(instance()._con);
		}
	}
	
};



int create_db()
{
  MYSQL *con = Mysql::instance().getMysql();
  if(!con)
	return -1;  

  if (mysql_query(con, "CREATE DATABASE  IF NOT EXISTS testdb")) 
  {
       return finish_with_error(con);
  }

  return 0;
}

int insert_data()
{
	
  MYSQL *con = Mysql::instance().getMysql();
  
  if (!con) 
  {
      fprintf(stderr, "%s\n", mysql_error(con));
      return -1;
  }  

   
  if (mysql_query(con, "DROP TABLE IF EXISTS Cars")) {
      return finish_with_error(con);
  }
  
  if (mysql_query(con, "CREATE TABLE Cars(Id INT, Name TEXT, Price INT)")) {      
      return finish_with_error(con);
  }
  
  if (mysql_query(con, "INSERT INTO Cars VALUES(1,'Audi',52642)")) {
      return finish_with_error(con);
  }
  
  if (mysql_query(con, "INSERT INTO Cars VALUES(2,'Mercedes',57127)")) {
      return finish_with_error(con);
  }
  
  if (mysql_query(con, "INSERT INTO Cars VALUES(3,'Skoda',9000)")) {
      return finish_with_error(con);
  }
  
  if (mysql_query(con, "INSERT INTO Cars VALUES(4,'Volvo',29000)")) {
      return finish_with_error(con);
  }
  
  if (mysql_query(con, "INSERT INTO Cars VALUES(5,'Bentley',350000)")) {
      return finish_with_error(con);
  }
  
  if (mysql_query(con, "INSERT INTO Cars VALUES(6,'Citroen',21000)")) {
      return finish_with_error(con);
  }
  
  if (mysql_query(con, "INSERT INTO Cars VALUES(7,'Hummer',41400)")) {
      return finish_with_error(con);
  }
  
  if (mysql_query(con, "INSERT INTO Cars VALUES(8,'Volkswagen',21600)")) {
      return finish_with_error(con);
  }

  return 0;
}

struct car_info
{
	string id;
	string name;
	string price;
};

int fetch_data(list<car_info*> &cars)
{
	MYSQL *con = Mysql::instance().getMysql();
  
  if (!con) 
  {
      fprintf(stderr, "%s\n", mysql_error(con));
      return -1;
  }  
	if (mysql_query(con, "SELECT * FROM Cars")) 
  {
      finish_with_error(con);
  }
  
  MYSQL_RES *result = mysql_store_result(con);
  
  if (result == NULL) 
  {
      finish_with_error(con);
  }

  int num_fields = mysql_num_fields(result);

  MYSQL_ROW row;
  
  while ((row = mysql_fetch_row(result))) 
  { 
	  car_info* pcar = new car_info;
	  pcar->id = row[0];
	  pcar->name = row[1];
	  pcar->price = row[2];
	  cars.push_back(pcar);
  }
  
  mysql_free_result(result);
  
  return 0;
}


void show_cars(list<car_info*> &cars)
{
	for(list<car_info*>::iterator it = cars.begin(); it != cars.end(); it++)
	{
		car_info* pcar = *it;
		cout<<"id:"<<pcar->id<<'\t' << "name:"<< pcar->name<<'\t' << "price:" <<pcar->price <<endl;
	}
}

int main(int argc, char **argv)
{
	connect_info info;
	/*
	 * 程序配置初始化
	 */
	 
	Mysql::instance().init(info);
	if(create_db() !=0)
		return -1;
	if(insert_data() != 0)
		return -1;
	list<car_info*> cars;
	if(fetch_data(cars) != 0)
		return -1;
		
	if(!cars.empty())
	{
		//todo
		
		show_cars(cars);
		
		for(list<car_info*>::iterator it = cars.begin(); it != cars.end(); it++)
		{
			car_info* pcar = *it;
			delete pcar;
		}
	}
	Mysql::instance().destroy();
    return 0;
}
