# 功能测试 [basic]

create table t_basic(id int, age int, name char, score float);

insert into t_basic values(1,1, 'a', 1.0);

insert into t_basic values(2,2, 'b', 2.0);

insert into t_basic values(4,4, 'c', 3.0);

insert into t_basic values(3,3, 'd', 4.0);

insert into t_basic values(5,5, 'e', 5.5);

insert into t_basic values(6,6, 'f', 6.6);

insert into t_basic values(7,7, 'g', 7.7);

select * from t_basic;

delete from t_basic where id=3;

select * from t_basic;

select * from t_basic where id=1;

select * from t_basic where id>=5;

select * from t_basic where age>1 and age<3;

select * from tbasic where tbasic.id=1 and t_basic.age=1;

select * from t_basic where id=1 and age=1;

select id, age, name, score from t_basic;

select tbasic.id, tbasic.age, tbasic.name, tbasic.score from t_basic;

select tbasic.id, tbasic.age, name from t_basic;

create index iid on t_basic (id);

select * from t_basic;

# 元数据校验 [select meta]/[insert]

drop table t1;

drop table t2;

create table t1(a int, b float, c char, d date);

create table t2(a int, b float, c char, d date);

insert into t1 values("1", 1.1, "lyq", "2020-10-22");

insert into t1 values(1, "1.1", "lyq", "2020-10-22");

insert into t1 values(1, 1.1, 111, "2020-10-22");

insert into t1 values(1, 1.1, 11.1, "2020-10-22");

drop table t1;

drop table t2;

# 功能测试 [drop table]

drop table t1;

drop table t2;

desc t1;

desc t2;

select * from t1;

select * from t2;

create table t1(a int, b float, c char, d date);

create table t2(a int, b float, c char, d date);

insert into t1 values(1, 1.1, "lyq", "2020-10-22");

insert into t1 values(11.1, 111, "com", "2020-10-9");

insert into t2 values(2, 2.2, "lhc", "2000-1-22");

insert into t2 values(22, 22.22, "com", "2000-1-1");

insert into t2 values(22.2, 222, "cn", "2000-12-12");

# 功能测试 [update]

update t1 set a = 3 where b < 2;

update t1 set a = 2 where t1.b < 2;

update t1 set a = 2 where t2.b < 2;

update t1 set a = "2" where b < 2;

update t1 set d = "2000-2-29";

update t1 set a = 111 where d < "2021-2-2";

update t1 set b = 222 where d = "2020-10-22";

update t1 set d = "1997-08-4";

update t1 set c = "lhc" where d = "1997-08-04";

update t1 set d = "1997-7-02" where b = 222;

update t1 set a = 11111 where u > -1;

update t1 set a = 11111 where b < 11111;

update t1 set b = 111.1 where a > 0.000;

update t1 set d = "1997-02-29" where a > 1;

update t1 set d = "1200-02-2" where a > 1;

select * from t1;

# 功能测试 [date]

### update

drop table t1;

drop table t2;

create table t1(a int, b float, c char, d date);

create table t2(a int, b float, c char, d date);

insert into t1 values(1, 1.1, "lyq", "2020-10-22");

insert into t1 values(11.1, 111, "com", "2020-10-9");

insert into t2 values(2, 2.2, "lhc", "2000-1-22");

insert into t2 values(22, 22.22, "com", "2000-1-1");

insert into t2 values(22.2, 222, "cn", "2000-12-12");

update t1 set d = "2000-1-1";

update t1 set d = "2000-1-2" where a > 1;

update t1 set d = "2020-2-29" where b < 2;

update t1 set a = 2000 where d < "2020-1-1";

update t1 set b = 1 where d = "2000-1-2";

update t1 set c = "2" where d <= "2020-1-2";

update t1 set a = 2020 where d = "2020-2-29" and d = "2020-2-29";

update t1 set b = 2 where "2020-2-29" = d and "2020-2-29" = d;

update t1 set c = "29" where d= "2020-2-29" and a = 2020 and b > 1;

select * from t1;

update t1 set d = "2000-2-29";

update t1 set d = "lksajdfa";

update t1 set a = 2020 where d > "2020-2-111";

update t1 set a = 2020 where d > 1;

update t1 set a = 2020 where d < 11.1;

update t1 set a = 2020 where d > "akldjie";

update t1 set "2000-2-29" = d;

update t1 set "lksajdfa" = d;

update t1 set a = 2020 where "2020-2-111" = d;

update t1 set a = 2020 where 1 < d;

update t1 set a = 2020 where 11.1 > d;

update t1 set a = 2020 where "akldjie" < d;

### select

drop table t1;

drop table t2;

create table t1(a int, b float, c char, d date);

create table t2(a int, b float, c char, d date);

insert into t1 values(1, 1.1, "lyq", "2020-10-22");

insert into t1 values(11.1, 111, "com", "2020-10-9");

insert into t2 values(2, 2.2, "lhc", "2000-1-22");

insert into t2 values(22, 22.22, "com", "2000-1-1");

insert into t2 values(22.2, 222, "cn", "2000-12-12");

select * from t1;

select a, b, c, d from t1;

select t1.a, t1.b, t1.c, t1.d from t1;

select *, a from t1;

select *, t1.a from t1;

select t1.*, a from t1;

select t2.a, t1.b, t2.c, d from t1;

select t1.*, t2.* from t1;

select t2.*, a from t1;

select t2.*, * from t1;

select * from t3;

select * from t1 where t1.a > 1;

select * from t1 where a > 2;

select * from t1 where 2 < a;

select * from t1 where b < a;

select * from t1 where d < "2038-1-1";

select * from t1 where t1.d < "2028-1-1";

select * from t1 where t2.a < 2;

select * from t1 where t2.b < t1.a;

### aggregate

select max(d) from t1;

select min(d) from t1;

select count(d) from t1;

select max(d), min(d), count(d) from t1;

select avg(d) from t1;



# 功能测试 [select-tables]

drop table t1;

drop table t2;

create table t1(a int, b float, c char, d date);

create table t2(a int, b float, c char, d date);

insert into t1 values(1, 1.1, "lyq", "2020-10-22");

insert into t1 values(11.1, 111, "com", "2020-10-9");

insert into t2 values(2, 2.2, "lhc", "2000-1-22");

insert into t2 values(22, 22.22, "com", "2000-1-1");

insert into t2 values(22.2, 222, "cn", "2000-12-12");

select t1.a, t2.b, t1.c, t2.d from t1, t2;

select t1.a, t1.*, t2.*, *, t1.a, *, t2.b from t1, t2;

select * from t2, t1;



select a, b, c, d from t1, t2;

select t3.a, t1.b, t4.c, t2.d from t1, t2;

select t3.* from t1, t2;



select t1.a, t1.b, t2.c, t2.d from t1, t2 where t1.a > t2.b and t1.a < 100000 and t2.b < 30000;



-- select * from t1, t2 where t3.a < t2.b;

-- select * from t1, t2 where a < b;



# 功能测试 [aggregate]

drop table t1;

drop table t2;

create table t1(a int, b float, c char, d date);

create table t2(a int, b float, c char, d date);

insert into t1 values(2, 1.1, "lyq", "2020-10-22");

insert into t1 values(11.1, 111, "com", "2020-10-9");

insert into t2 values(2, 2.2, "lhc", "2000-1-22");

insert into t2 values(22, 22.22, "com", "2000-1-1");

insert into t2 values(22.2, 222, "cn", "2000-12-12");

select * from t1;

select * from t2;

select * from t1, t2;



select max(t2.a), max(t1.a) from t1, t2;

select min(t2.a), min(t1.a) from t1, t2;

select count(1), count(*), count(t2.a), count(t1.a) from t1, t2;

select avg(t1.a), avg(t1.b) from t1, t2;



select max(a), min(b) from t1 where t1.b > 2;

select max(t1.a) from t1, t2 where t1.a > 1 and t2.a < 22;

select max(1.1) from t1;

select max(c) from t2;

select max('c') from t2;

select min(c) from t2;

select min('c') from t2;

