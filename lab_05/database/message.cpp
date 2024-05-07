#include "message.h"
#include "database.h"

#include <sstream>

#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

namespace database
{
    Message Message::fromJSON(const std::string &str)
    {
        int start = str.find("_id");
        int end = str.find(",",start);

        std::string s1 = str.substr(0,start-1);
        std::string s2 = str.substr(end+1);

        Message message;

        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(s1+s2);
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

        message.message_id() = object->getValue<long>("message_id");
        message.sender_id() = object->getValue<long>("sender_id");
        message.recipient_id() = object->getValue<long>("recipient_id");
        message.text() = object->getValue<std::string>("text");
        message.timestamp() = object->getValue<std::string>("timestamp");
        message.is_read() = object->getValue<bool>("is_read");
        Poco::JSON::Object::Ptr object_array = object->getObject("attachment");
        message._attachment.type = object_array->getValue<std::string>("type");
        message._attachment.url = object_array->getValue<std::string>("url");

        return message;
    }

    long Message::get_message_id() const
    {
        return _message_id;
    }

    long Message::get_sender_id() const
    {
        return _sender_id;
    }

    long Message::get_recipient_id() const
    {
        return _recipient_id;
    }

    const std::string &Message::get_text() const
    {
        return _text;
    }

    const std::string &Message::get_timestamp() const
    {
        return _timestamp;
    }

    bool Message::get_is_read() const
    {
        return _is_read;
    }

    const std::string  Message::get_attachment_as_string() const
    {
        return _attachment.to_string();
    }

    const std::string Message::Attachment::to_string() const
    {
        std::stringstream ss;
        ss << type << ", " << url;
        return ss.str();
    }

    long &Message::message_id()
    {
        return _message_id;
    }

    long &Message::sender_id()
    {
        return _sender_id;
    }

    long &Message::recipient_id()
    {
        return _recipient_id;
    }

    std::string &Message::text()
    {
        return _text;
    }

    std::string &Message::timestamp()
    {
        return _timestamp;
    }

    bool &Message::is_read()
    {
        return _is_read;
    }

    Message::Attachment &Message::attachment()
    {
        return _attachment;
    }

    std::optional<Message> Message::read_by_id(long message_id)
    {
        std::optional<Message> result;
        std::map<std::string,long> params;
        params["message_id"] = message_id;
        std::vector<std::string> results = database::Database::get().get_from_mongo("arch.messages",params);

        if(!results.empty())
            result = fromJSON(results[0]);
        
        return result;
    }

    std::vector<Message> Message::read_by_recipient_id(long recipient_id)
    {
        std::vector<Message> result;
        std::map<std::string,long> params;
        params["recipient_id"] = recipient_id;

        std::vector<std::string> results = database::Database::get().get_from_mongo("arch.messages",params);

        for(std::string& s : results) 
            result.push_back(fromJSON(s));
        

        return result;
    }

    void Message::add()
    {
        database::Database::get().send_to_mongo("messages",toJSON());
    }

    void Message::update()
    {
        std::map<std::string,long> params;
        params["message_id"] = _message_id;       
        database::Database::get().update_mongo("messages",params,toJSON());
    }

    Poco::JSON::Object::Ptr Message::toJSON() const
    {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("message_id", _message_id);
        root->set("sender_id", _sender_id);
        root->set("recipient_id", _recipient_id);
        root->set("text", _text);
        root->set("timestamp", _timestamp);
        root->set("is_read", _is_read);

        Poco::JSON::Object::Ptr attachment = new Poco::JSON::Object();

        attachment->set("type",_attachment.type);
        attachment->set("url",_attachment.url);

        root->set("attachment", attachment);


        return root;
    }
}