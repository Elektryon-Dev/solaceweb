//
// Created by mwo on 8/12/16.
//

#ifndef RESTBED_XMR_YOURMONEROREQUESTS_H
#define RESTBED_XMR_YOURMONEROREQUESTS_H

#include <iostream>
#include <functional>

#include "tools.h"

#include "../ext/restbed/source/restbed"

namespace xmreg
{

using namespace std;
using namespace restbed;
using namespace nlohmann;


string
get_current_time(const char* format = "%a, %d %b %Y %H:%M:%S %Z")
{

    auto current_time = date::make_zoned(
            date::current_zone(),
            date::floor<chrono::seconds>(std::chrono::system_clock::now())
    );

    return date::format(format, current_time);
}


multimap<string, string>
make_headers(const multimap<string, string>& extra_headers = multimap<string, string>())
{
    multimap<string, string> headers {
            {"Date", get_current_time()},
            {"Access-Control-Allow-Origin",      "http://127.0.0.1"},
            {"access-control-allow-headers",     "*, DNT,X-CustomHeader,Keep-Alive,User-Agent,X-Requested-With,If-Modified-Since,Cache-Control,Content-Type,Set-Cookie"},
            {"access-control-max-age",           "86400, 1728000"},
            {"access-control-allow-methods",     "GET, POST, OPTIONS"},
            {"access-control-allow-credentials", "true"},
            {"Content-Type",                     "application/json"}
    };

    headers.insert(extra_headers.begin(), extra_headers.end());

    return headers;
};

struct handel_
{
    using fetch_func_t = function< void ( const shared_ptr< Session >, const Bytes& ) >;

    fetch_func_t request_callback;

    handel_(const fetch_func_t& callback):
            request_callback {callback}
    {}

    void operator()(const shared_ptr< Session > session)
    {
        const auto request = session->get_request( );
        size_t content_length = request->get_header( "Content-Length", 0);
        session->fetch(content_length, this->request_callback);
    }
};


class YourMoneroRequests
{

public:

    static bool show_logs;

    YourMoneroRequests()
    {}

    void
    login(const shared_ptr< Session > session, const Bytes & body)
    {
        json j_request = body_to_json(body);

        if (show_logs)
            print_json_log("login request: ", j_request);

        json j_response {{"new_address", true}};

        string response_body = j_response.dump();

        auto response_headers = make_headers({{ "Content-Length", to_string(response_body.size())}});

        session->close( OK, response_body, response_headers);
    }

    void
    get_address_txs(const shared_ptr< Session > session, const Bytes & body)
    {
        json j_request = body_to_json(body);

        if (show_logs)
            print_json_log("get_address_txs request: ", j_request);

        json j_response {
                { "total_received", "0"},
                { "scanned_height", 2012455},
                { "scanned_block_height", 1195848},
                { "start_height", 2012455},
                { "transaction_height", 2012455},
                { "blockchain_height", 1195848}
        };

        string response_body = j_response.dump();

        auto response_headers = make_headers({{ "Content-Length", to_string(response_body.size())}});

        session->close( OK, response_body, response_headers);
    }

    void
    get_address_info(const shared_ptr< Session > session, const Bytes & body)
    {
        json j_request = body_to_json(body);

        if (show_logs)
            print_json_log("get_address_info request: ", j_request);

        json j_response  {
                {"locked_funds", "0"},
                {"total_received", "0"},
                {"total_sent", "0"},
                {"scanned_height", 2012470},
                {"scanned_block_height", 1195850},
                {"start_height", 2012470},
                {"transaction_height", 2012470},
                {"blockchain_height", 1195850},
                {"spent_outputs", nullptr}
        };

        string response_body = j_response.dump();

        auto response_headers = make_headers({{ "Content-Length", to_string(response_body.size())}});

        session->close( OK, response_body, response_headers);
    }

    shared_ptr<Resource>
    make_resource(function< void (YourMoneroRequests&, const shared_ptr< Session >, const Bytes& ) > handle_func,
                  const string& path)
    {
        auto a_request = std::bind(handle_func, *this, std::placeholders::_1, std::placeholders::_2);

        shared_ptr<Resource> resource_ptr = make_shared<Resource>();

        resource_ptr->set_path(path);
        resource_ptr->set_method_handler( "OPTIONS", generic_options_handler);
        resource_ptr->set_method_handler( "POST"   , handel_(a_request) );

        return resource_ptr;
    }


    static void
    generic_options_handler( const shared_ptr< Session > session )
    {
        const auto request = session->get_request( );

        size_t content_length = request->get_header( "Content-Length", 0);

        //cout << "generic_options_handler" << endl;

        session->fetch(content_length, [](const shared_ptr< Session > session, const Bytes & body)
        {
            session->close( OK, string{}, make_headers());
        });
    }

    static void
    print_json_log(const string& text, const json& j)
    {
       cout << text << '\n' << j.dump(4) << endl;
    }


    static inline string
    body_to_string(const Bytes & body)
    {
        return string(reinterpret_cast<const char *>(body.data()), body.size());
    }

    static inline json
    body_to_json(const Bytes & body)
    {
        json j = json::parse(body_to_string(body));
        return j;
    }

};

bool YourMoneroRequests::show_logs = false;

}
#endif //RESTBED_XMR_YOURMONEROREQUESTS_H
