#include "database.h"
#include "loggerwrapper.h"

std::unique_ptr<Database> Database::db = nullptr;

Database& Database::get_connection(const std::string& conn_string) {
    if (!db) {
        db = std::unique_ptr<Database>(new Database(conn_string));
        return *db;
    }
    return *db;
}

Database::Database(const std::string& conn_string) {
    conn = std::make_unique<pqxx::connection>(conn_string);
    if (conn->is_open()) {
        std::cout << "connected successfully\n";
    }
}


bool Database::insert(const std::string& insert_query) {
    try {
        if (!conn || !conn->is_open()) {
            std::cerr << "conn object is not valid\n";
            return false;
        }
        // Start a transaction
        pqxx::work txn(*conn);
        txn.exec("CREATE TABLE IF NOT EXISTS orders("
        "order_id int, "
        "account_id int,"
        "instrument_id int,"
        "quantity int,"
        "price int,"
        "symbol text,"
        "side text)"
        );
        txn.exec(insert_query);
        std::cout << "Inserted a user (if not exists).\n";
        txn.commit();
    }
    catch (const pqxx::sql_error& error) {
        std::cerr
            << "Database error: " << error.what() << std::endl
            << "Query was: " << error.query() << std::endl;
        return false;
    }
    catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
        return false;
    }
    return true;
    }

