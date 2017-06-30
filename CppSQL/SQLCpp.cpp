// SQLCpp.cpp : �������̨Ӧ�ó������ڵ㡣
//


#include <string>
#include <iostream>

#include <conio.h>
#include <stdarg.h> 

using namespace std;

#include "sql.h"
using namespace sql;


/*
	//C99�ɱ䳤�����ĺ�����Ҫ�ÿ��ַ���("")Ϊ���һ��������nullptr�������м�Ĳ�����Ҫ�п��ַ���("")
*/


int main()
{

	InsertModel i;
	i.insert("score", 100)
		("name", std::string("six"))
		("age", (unsigned char)20)
		("address", "beijing")
		("create_time", nullptr)
		.into("user");
	std::cout << i.str() << std::endl;

	SelectModel s;
	s.select("id", "age", "name", "address","")
		.from("user","")
		.where(column("score") > 60 && (column("age") >= 20 || column("address").is_not_null()))
		.group_by("age","")
		.having(column("age") > 10)
		.order_by("age desc")
		.limit(10)
		.offset(1);
	std::cout << s << std::endl;
	// select id, age, name, address from user where (score > 60) and ((age >= 20) or (address is not null)) group by age having age > 10 order by age desc limit 10 offset 1

	//C++11 std::vector<int> a = { 1, 2, 3 };
	int v1[] = { 1, 2, 3 };
	std::vector<int> a(&v1[0],&v1[2]);

	UpdateModel u;
	u.update("user")
		.set("name", "ddc")
		("age", 18)
		("score", nullptr)
		("address", "beijing")
		.where(column("id").in(a));
	std::cout << u << std::endl;
	 //update user set name = 'ddc', age = 18, score = 18, address = 'beijing' where id in (1, 2, 3)

	DeleteModel d;
	d._delete().from("user","").where(column("id") == 1);
	std::cout << d << std::endl;

	system("pause");
    return 0;
}

