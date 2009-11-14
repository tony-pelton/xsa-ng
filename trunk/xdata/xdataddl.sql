connect 'jdbc:derby://localhost/data;create=true' user 'sa' password 'sa';

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

disconnect;

exit;

-- commit;

-- create index idx_st_alpha on nationaldata(st_alpha);
-- create index idx_lat on nationaldata(lat);
-- create index idx_lon on nationaldata(lon);
-- create index idx_featureid on nationaldata(featureid);