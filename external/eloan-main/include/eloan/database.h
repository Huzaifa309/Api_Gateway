//
// Created by muhammad-abdullah on 7/15/25.
//

#ifndef DATABASE_H
#define DATABASE_H

#include<iostream>
#include<pqxx/pqxx>

//
// Created by muhammad-abdullah on 7/15/25.
//

class Database {
public:
    static Database& get_connection(const std::string&);
    bool insert(const std::string&);
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;
    ~Database() = default;
private:
    explicit Database(const std::string&);
    static std::unique_ptr<Database> db;
    std::unique_ptr<pqxx::connection> conn;
    std::string conn_string;
};

#endif //DATABASE_H
