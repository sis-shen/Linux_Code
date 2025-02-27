DROP database IF EXISTS testDB;

CREATE database testDB DEFAULT CHARACTER SET utf8mb4;

use testDB;

CREATE TABLE users(name varchar(255),id int)ENGINE=INNODB DEFAULT CHARSET=utf8mb4;

INSERT INTO users VALUES ("sup",1),("nick",2);