-- 
-- This SQL file is used to create the RHESsys parameter database
-- using the following command:
-- 
--     sqlite3 rhessys-params.sqlite < createParamDB.sql
--

create table admin(
    id integer primary key autoincrement,
    dt datetime,
    user text not null,
    command text not null
);

-- Parameter class type table
-- These are entered manually when the database is initialized
-- and each new class that is entered into the class table must have the class type defined here.
create table class_type(
    -- type_id integer primary key autoincrement,
    type_id integer primary key,
    type_name text not null,
    max_id integer not null
);

-- Type of parameter class: "basin", "landuse", "surface_energy", "fire", "soil", "zone", "hillslope", "stratum"
insert into class_type (type_id, type_name, max_id) values (1, 'basin', 0);
insert into class_type (type_id, type_name, max_id) values (2, 'landuse', 0);
insert into class_type (type_id, type_name, max_id) values (3, 'surface_energy', 0);
insert into class_type (type_id, type_name, max_id) values (4, 'fire', 0);
insert into class_type (type_id, type_name, max_id) values (5, 'soil', 0);
insert into class_type (type_id, type_name, max_id) values (6, 'zone', 0);
insert into class_type (type_id, type_name, max_id) values (7, 'hillslope', 0);
insert into class_type (type_id, type_name, max_id) values (8, 'stratum', 0);

-- Parameter classes table
-- Classes are defined as the class name, e.g. "Douglas Fir" combined with the
-- location, e.g. "", "Oregon", "Western United States"
create table class(
    class_id integer primary key autoincrement,
    name text not null,
    location text not null,
    type_id integer not null,
    genus text not null,
    species text not null,
    default_id integer not null, 
    parent_id integer  not null references class(class_id), -- self referential (parent id has to be entered before child id)
    foreign key(type_id) references class_type(type_id),
    unique (name, location)
    unique (type_id, default_id)
);

-- Populate the class tqble with known base classes
-- Soil types
-- Note: using autoincrement to determine 'class_id'. The 'type_id' values are hard-coded from the class_type table initialization above.
insert into class (name, location, type_id, genus, species, default_id, parent_id) values ('clay',            '', 5, '', '', 1, 0);
insert into class (name, location, type_id, genus, species, default_id, parent_id) values ('silty-clay',      '', 5, '', '', 2, 0);
insert into class (name, location, type_id, genus, species, default_id, parent_id) values ('silty-clay-loam', '', 5, '', '', 3, 0);
insert into class (name, location, type_id, genus, species, default_id, parent_id) values ('sandy-clay',      '', 5, '', '', 4, 0);
insert into class (name, location, type_id, genus, species, default_id, parent_id) values ('sandy-clay-loam', '', 5, '', '', 5, 0);
insert into class (name, location, type_id, genus, species, default_id, parent_id) values ('clay-loam',       '', 5, '', '', 6, 0);
insert into class (name, location, type_id, genus, species, default_id, parent_id) values ('silt',            '', 5, '', '', 7, 0);
insert into class (name, location, type_id, genus, species, default_id, parent_id) values ('silt-loam',       '', 5, '', '', 8, 0);
insert into class (name, location, type_id, genus, species, default_id, parent_id) values ('loam',            '', 5, '', '', 9, 0);
insert into class (name, location, type_id, genus, species, default_id, parent_id) values ('sand',            '', 5, '', '', 10, 0);
insert into class (name, location, type_id, genus, species, default_id, parent_id) values ('loamy-sand',      '', 5, '', '', 11, 0);
insert into class (name, location, type_id, genus, species, default_id, parent_id) values ('sandy-loam',      '', 5, '', '', 12, 0);
-- Stratum types
insert into class (name, location, type_id, genus, species, default_id, parent_id) values ('deciduous', '', 8, '', '', 1, 0);
insert into class (name, location, type_id, genus, species, default_id, parent_id) values ('conifer',   '', 8, '', '', 2, 0);
insert into class (name, location, type_id, genus, species, default_id, parent_id) values ('grass',     '', 8, '', '', 3, 0);
insert into class (name, location, type_id, genus, species, default_id, parent_id) values ('nonveg',    '', 8, '', '', 4, 0);
insert into class (name, location, type_id, genus, species, default_id, parent_id) values ('algae',     '', 8, '', '', 5, 0);

-- Parameter table
create table param(
    class_id integer not null,
    name text not null,
    value text not null,
    dt datetime not null,
    reference text not null,
    comment text not null,
    user text not null,
    primary key(class_id, name, value),
    foreign key(class_id) references class(class_id) on delete cascade
);

-- Views
-- View of both class table and param table
create view class_param as select t.type_name,c.default_id,c.name,c.location,c.genus,c.species,c.parent_id,p.name, 
   p.value, p.dt, p.reference, p.comment, p.user from class_type t, class c, param p where t.type_id=c.type_id and c.class_id=p.class_id;
