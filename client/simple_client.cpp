#include "simple_client.hpp"

std::optional<std::string> SimpleClient::Get(const std::string& key) {
  std::shared_ptr<ServerConn> conn = connect_to_server(this->server_addr);
  if (!conn) {
    cerr_color(RED, "Failed to connect to KvServer at ", this->server_addr,
               '.');
    return std::nullopt;
  }

  GetRequest req{key};
  if (!conn->send_request(req)) return std::nullopt;

  std::optional<Response> res = conn->recv_response();
  if (!res) return std::nullopt;
  if (auto* get_res = std::get_if<GetResponse>(&*res)) {
    return get_res->value;
  } else if (auto* error_res = std::get_if<ErrorResponse>(&*res)) {
    cerr_color(RED, "Failed to Get value from server: ", error_res->msg);
  }

  return std::nullopt;
}

bool SimpleClient::Put(const std::string& key, const std::string& value) {
  std::shared_ptr<ServerConn> conn = connect_to_server(this->server_addr);
  if (!conn) {
    cerr_color(RED, "Failed to connect to KvServer at ", this->server_addr,
               '.');
    return false;
  }

  PutRequest req{key, value};
  if (!conn->send_request(req)) return false;

  std::optional<Response> res = conn->recv_response();
  if (!res) return false;
  if (auto* put_res = std::get_if<PutResponse>(&*res)) {
    return true;
  } else if (auto* error_res = std::get_if<ErrorResponse>(&*res)) {
    cerr_color(RED, "Failed to Put value to server: ", error_res->msg);
  }

  return false;
}

bool SimpleClient::Append(const std::string& key, const std::string& value) {
  std::shared_ptr<ServerConn> conn = connect_to_server(this->server_addr);
  if (!conn) {
    cerr_color(RED, "Failed to connect to KvServer at ", this->server_addr,
               '.');
    return false;
  }

  AppendRequest req{key, value};
  if (!conn->send_request(req)) return false;

  std::optional<Response> res = conn->recv_response();
  if (!res) return false;
  if (auto* append_res = std::get_if<AppendResponse>(&*res)) {
    return true;
  } else if (auto* error_res = std::get_if<ErrorResponse>(&*res)) {
    cerr_color(RED, "Failed to Append value to server: ", error_res->msg);
  }

  return false;
}

std::optional<std::string> SimpleClient::Delete(const std::string& key) {
  std::shared_ptr<ServerConn> conn = connect_to_server(this->server_addr);
  if (!conn) {
    cerr_color(RED, "Failed to connect to KvServer at ", this->server_addr,
               '.');
    return std::nullopt;
  }

  DeleteRequest req{key};
  if (!conn->send_request(req)) return std::nullopt;

  std::optional<Response> res = conn->recv_response();
  if (!res) return std::nullopt;
  if (auto* delete_res = std::get_if<DeleteResponse>(&*res)) {
    return delete_res->value;
  } else if (auto* error_res = std::get_if<ErrorResponse>(&*res)) {
    cerr_color(RED, "Failed to Delete value on server: ", error_res->msg);
  }

  return std::nullopt;
}

std::optional<std::vector<std::string>> SimpleClient::MultiGet(
    const std::vector<std::string>& keys) {
  std::shared_ptr<ServerConn> conn = connect_to_server(this->server_addr);
  if (!conn) {
    cerr_color(RED, "Failed to connect to KvServer at ", this->server_addr,
               '.');
    return std::nullopt;
  }

  MultiGetRequest req{keys};
  if (!conn->send_request(req)) return std::nullopt;

  std::optional<Response> res = conn->recv_response();
  if (!res) return std::nullopt;
  if (auto* multiget_res = std::get_if<MultiGetResponse>(&*res)) {
    return multiget_res->values;
  } else if (auto* error_res = std::get_if<ErrorResponse>(&*res)) {
    cerr_color(RED, "Failed to MultiGet values on server: ", error_res->msg);
  }

  return std::nullopt;
}

bool SimpleClient::MultiPut(const std::vector<std::string>& keys,
                            const std::vector<std::string>& values) {
  std::shared_ptr<ServerConn> conn = connect_to_server(this->server_addr);
  if (!conn) {
    cerr_color(RED, "Failed to connect to KvServer at ", this->server_addr,
               '.');
    return false;
  }

  MultiPutRequest req{keys, values};
  if (!conn->send_request(req)) return false;

  std::optional<Response> res = conn->recv_response();
  if (!res) return false;
  if (auto* multiput_res = std::get_if<MultiPutResponse>(&*res)) {
    return true;
  } else if (auto* error_res = std::get_if<ErrorResponse>(&*res)) {
    cerr_color(RED, "Failed to MultiPut values on server: ", error_res->msg);
  }

  return false;
}

bool SimpleClient::GDPRDelete(const std::string& user) {
  // Retrieves a comma-separated list of post_ids that the user has posted
  std::optional<std::string> list_of_post_ids = Get(user + "_posts");

  // Checks if the list of post IDs is null
  if (list_of_post_ids == std::nullopt) {
    return true;
  }
  // Checks if the list of post IDs is NOT null
  else {
    // Remove whitespace from the list of post IDs
    list_of_post_ids.value().erase(std::remove_if(list_of_post_ids.value().begin(), 
                                                  list_of_post_ids.value().end(), 
                                                  std::bind(std::isspace<char>, std::placeholders::_1, std::locale::classic())), 
                                  list_of_post_ids.value().end());
    
    // Iterate through the resulting string, extract each post ID, and push it into a vector
    std::vector<std::string> post_ids;
    // Initialize a string stream to be used for parsing each post ID
    std::stringstream stream(list_of_post_ids.value());

    // Iterate through the string stream and add each post ID to the vector we created
    while (stream.good()) {
      std::string post;
      // Uses a comma as a delimiter
      getline(stream, post, ',');
      post_ids.push_back(post);
    }
    // Loop through the vector and delete the post content associated with a particular post ID
    for (int j = 0; j < post_ids.size(); j++) {
      std::optional<std::string> delete_response_1 = Delete(post_ids[j]);
    }

    // Delete the list of post IDs the user has posted
    std::optional<std::string> delete_response_2 = Delete(user + "_posts");

    // Delete the user from the list of users
    std::optional<std::string> list_of_user_ids = Get("all_users");

    // Remove whitespace from the string containing all users
    list_of_user_ids.value().erase(std::remove_if(list_of_user_ids.value().begin(), 
                                                  list_of_user_ids.value().end(), 
                                                  std::bind(std::isspace<char>, std::placeholders::_1, std::locale::classic())), 
                                  list_of_user_ids.value().end());

    // Replace the input user ID in all_users with an empty string, effectively removing it
    std::size_t i = list_of_user_ids.value().find(user);
    if (i == list_of_user_ids.value().length() - user.length() -1) {
      list_of_user_ids.value().replace(i, user.length(), "");
    }
    else {
      list_of_user_ids.value().replace(i, user.length() + 1, "");
    }
    // Input the new KV pair for list_of_user_ids back into the map
    Put("all_users", list_of_user_ids.value());

    std::optional<std::string> delete_response_3 = Delete(user);
    return true;
  }
}