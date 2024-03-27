#include "post.h"
#include "database.h"
#include "../config/config.h"

#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/RecordSet.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

#include <sstream>
#include <exception>

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

namespace database
{

    void Post::init()
    {
        try
        {

            Poco::Data::Session session = database::Database::get().create_session();
            Statement create_stmt(session);
            create_stmt << "CREATE TABLE IF NOT EXISTS posts (id SERIAL PRIMARY KEY,"
                        << "user_id INTEGER NOT NULL,"
                        << "post_text VARCHAR(1024) NOT NULL,"
                        << "date_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
                        << "media VARCHAR(256) NULL);",
                now;
        }

        catch (Poco::Data::PostgreSQL::PostgreSQLException &e)
        {
            std::cout << "connection:" << e.displayText() << std::endl;
            throw;
        }
        catch (Poco::Data::ConnectionFailedException &e)
        {
            std::cout << "connection:" << e.displayText() << std::endl;
            throw;
        }
    }

    Poco::JSON::Object::Ptr Post::toJSON() const
    {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("id", _id);
        root->set("user_id", _user_id);
        root->set("post_text", _post_text);
        root->set("date_time", _date_time);
        root->set("media", _media);

        return root;
    }

    Post Post::fromJSON(const std::string &str)
    {
        Post post;
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(str);
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

        post.id() = object->getValue<long>("id");
        post.user_id() = object->getValue<long>("user_id");
        post.post_text() = object->getValue<std::string>("post_text");
        post.date_time() = object->getValue<std::string>("date_time");
        post.media() = object->getValue<std::string>("media");

        return post;
    }

    std::optional<Post> Post::read_by_id(long id)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement select(session);
            Post a;
            select << "SELECT id, user_id, post_text, date_time, media FROM posts where id=$1",
                into(a._id),
                into(a._user_id),
                into(a._post_text),
                into(a._date_time),
                into(a._media),
                use(id),
                range(0, 1); //  iterate over result set one row at a time

            select.execute();
            Poco::Data::RecordSet rs(select);
            if (rs.moveFirst())
                return a;
        }

        catch (Poco::Data::PostgreSQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
        }
        catch (Poco::Data::PostgreSQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
        }
        return {};
    }

    std::vector<Post> Post::read_by_user_id(long user_id)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement select(session);
            std::vector<Post> result;
            Post a;
            select << "SELECT id, user_id, post_text, date_time, media FROM posts where user_id=$1",
                into(a._id),
                into(a._user_id),
                into(a._post_text),
                into(a._date_time),
                into(a._media),
                use(user_id),
                range(0, 1); //  iterate over result set one row at a time

            while (!select.done())
            {
                if (select.execute())
                    result.push_back(a);
            }
            return result;
        }

        catch (Poco::Data::PostgreSQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
        }
        catch (Poco::Data::PostgreSQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
        }
        return {};
    }

    void Post::add()
    {

        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement insert(session);

            insert << "INSERT INTO posts (user_id, post_text, media) VALUES($1, $2, $3)",
                use(_user_id),
                use(_post_text),
                use(_media);

            insert.execute();

            Poco::Data::Statement select(session);
            select << "SELECT LASTVAL()",
                into(_id),
                range(0, 1); //  iterate over result set one row at a time

            if (!select.done())
            {
                select.execute();
            }
            std::cout << "inserted:" << _id << std::endl;
        }
        catch (Poco::Data::PostgreSQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::PostgreSQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    void Post::update()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement update(session);

            update << "UPDATE posts SET user_id = $1, post_text = $2, media = $3 WHERE id = $4",
                use(_user_id);
                use(_post_text),
                use(_media),
                use(_id);

            update.execute();

            std::cout << "updated: " << _id << std::endl;
        }
        catch (Poco::Data::PostgreSQL::ConnectionException &e)
        {
            std::cout << "connection: " << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::PostgreSQL::StatementException &e)
        {
            std::cout << "statement: " << e.what() << std::endl;
            throw;
        }
    }


    long Post::get_id() const
    {
        return _id;
    }

    long Post::get_user_id() const
    {
        return _user_id;
    }

    const std::string &Post::get_post_text() const
    {
        return _post_text;
    }

    const std::string &Post::get_date_time() const
    {
        return _date_time;
    }

    const std::string &Post::get_media() const
    {
        return _media;
    }

    long &Post::id()
    {
        return _id;
    }

    long &Post::user_id()
    {
        return _user_id;
    }

    std::string &Post::post_text()
    {
        return _post_text;
    }

    std::string &Post::date_time()
    {
        return _date_time;
    }

    std::string &Post::media()
    {
        return _media;
    }
}