// Copyright by Enigma

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdlib>
#include <iostream>
#include <string>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;


int main(int argc, char** argv) {
  try {
    if(argc != 5) {
      std::cerr << "Usage: http-client-sync <host> <port> <target> "
                   "<request>\n"
                << "Example:\n"
                   "localhost 8080 /v1/api/suggest "
                << R"({"input":"<user_input>"})"
                   "\n" << argv[1] << " " << argv[2]
                << " " << argv[3] << " " << argv[4];
      return EXIT_FAILURE;
    }
    auto const host = argv[1];
    auto const port = argv[2];
    auto const target = argv[3];
    auto const request = argv[4];
    int version = 11;
    std::cout << host << " " << port << " " << target
              << " " <<  request << std::endl;
    net::io_context ioc;

    tcp::resolver resolver(ioc);
    beast::tcp_stream stream(ioc);

    auto const results = resolver.resolve(host, port);

    stream.connect(results);

    http::string_body::value_type body = request;


    http::request<http::string_body> req{http::verb::post, target, version};
    req.set(http::field::host, host);
    req.body() = body;
    req.prepare_payload();
    req.set(http::field::content_type, "text/html");

    http::write(stream, req);

    beast::flat_buffer buffer;

    http::response<http::dynamic_body> res;

    http::read(stream, buffer, res);

    std::cout << res << std::endl;

    beast::error_code ec;
    stream.socket().shutdown(tcp::socket::shutdown_both, ec);

    if(ec && ec != beast::errc::not_connected)
      throw beast::system_error{ec};

  }
  catch(std::exception const& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
