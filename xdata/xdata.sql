create table nationaldata (
id int NOT NULL GENERATED ALWAYS AS IDENTITY,
featureid int,
featurename varchar(255),
class varchar(128),
st_alpha varchar(128),
st_num int,
county varchar(128),
county_num int,
lat numeric(10,6),
lon numeric(10,6),
s_lat numeric(10,6),
s_lon numeric(10,6),
height int,
map_name varchar(128)
);

create index idx_st_alpha on nationaldata(st_alpha)

create index idx_lat on nationaldata(lat)

create index idx_lon on nationaldata(lon)

create index idx_featureid on nationaldata(featureid)

call SYSCS_UTIL.SYSCS_COMPRESS_TABLE('ADMIN', 'NATIONALDATA', 0)

drop table outdata

create table outdata as select * from nationaldata with no data

drop view data_v

create view data_v (id,featureid,featurename,class,st_alpha,st_num,county,county_num,lat,lon,s_lat,s_lon,height,map_name,pri_good,src_good) as
 select id,featureid,featurename,class,st_alpha,st_num,county,county_num,lat,lon,s_lat,s_lon,height,map_name,
 case when height is null then 0 when height < 0 then 0 when lat = 0 then 0 when lat is null then 0 when lon = 0 then 0 when lon is null then 0 else 1 end as pri_good,
 case when s_lat = 0 then 0 when s_lat is null then 0 when s_lon = 0 then 0 when s_lon is null then 0 else 1 end as src_good
 from nationaldata

drop table latlon

create table latlon as select id,featureid,class,trim(char(lat))||'|'||trim(char(lon)) as latlon from nationaldata with no data

insert into latlon select id,featureid,class,trim(char(lat))||'|'||trim(char(lon)) as latlon from nationaldata

-- END DDL

delete from outdata

insert into outdata select * from nationaldata where st_alpha = 'AK'

select * from nationaldata where st_alpha = 'HI' and class = 'LOCALE'

select class,featurename,lat,lon,county,featureid,height from data_v
 where 1=1
 and st_alpha = 'ME'
 and pri_good = 1
 -- and class in ('LOCALE','POPULATED PLACE')
 and class in ('TOWER')
 and county in ('Cumberland')
 order by county,lat,lon

select
 a.featureid,
 a.class,
 a.featurename,
 -- a.lat,
 -- a.lon,
 b.featureid,
 b.class,
 b.featurename
 -- b.lat,
 -- b.lon
 from nationaldata a,nationaldata b
 where 1=1
 and a.lat = b.lat and a.lon = b.lon
 -- and a.id <> b.id
 and a.featureid < b.featureid
 and a.lat <> 0 and b.lat <> 0 and a.lon <> 0 and b.lon <> 0
 and a.st_alpha = 'AK' and b.st_alpha = 'AK'
 and a.class <> 'POST OFFICE' and b.class <> 'POST OFFICE'
 order by a.featurename,b.featurename

-- group by a.featureid
 -- order by a.lat,a.lon

select latlon,count(latlon) as cnt from latlon group by latlon having count(latlon) > 1

select count(*) from latlon

select * from data_v where st_alpha = 'ME' and class = 'POST OFFICE' and pri_good = 0