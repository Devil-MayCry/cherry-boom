//
//  main.cpp
//  cherryTest
//
//  Created by huteng on 17/1/9.
//  Copyright © 2017年 huteng. All rights reserved.
//
#include <stdio.h>
#include <iostream>
#include "include/drafter/drafter.h"
#include "include/jsoncpp/json-forwards.h"
#include "include/jsoncpp/json.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <regex>
#include "file_system.h"

void ControllerMaker(std::string groupName, std::string methodName) {
    std::string controllerFileName = "/tmp/"+ groupName +"Controller.ts";
    std::ofstream out(controllerFileName);
    std:: string file_input = haze::FileSystem::readFile("/Users/huteng/api-cherry/cherry-boom/cherry-boom/template/templatecontroller.ts");
    std::regex group_name_regex ("[\$][\{]group_name[\}]");
    std::regex method_name_regex ("[\$][\{]method_name[\}]");
    std::string result ="";
    result = std::regex_replace (file_input,group_name_regex, groupName);
    result = std::regex_replace (result, method_name_regex, methodName);

    //std::cout<<result+"\n";
        if (out.is_open())
        {
            out << result;
            out.close();
        }
}

void ServiceMaker(std::string groupName) {
    std::string serviceFileName = "/tmp/"+ groupName +"Service.ts";
    std::ofstream out(serviceFileName);
}

void RepositoryMaker(std::string groupName) {
    std::string repositoryFileName = "/tmp/"+ groupName +"Repository.ts";
    std::ofstream out(repositoryFileName);
}

Json::Value ConvertBluePrintToJson(const char* blue_print) {
    drafter_parse_options parse_options = {false};
    drafter_serialize_options options;
    options.sourcemap = false;
    options.format = DRAFTER_SERIALIZE_JSON;
    
    char* res = 0;
    drafter_parse_blueprint_to(blue_print, &res, parse_options, options);
    Json::Value value;
    Json::Reader reader;
    reader.parse(res,value);
    return value;
}

std::string GetGroupName( Json::Value json_value){
    const char * group_name = json_value["content"][0]["content"][0]["meta"]["title"].asString().c_str();
    char letter[100];
    strcpy(letter,group_name);

    const char *d = " ";
    char *p;
    p = strtok(letter,d);
    std::string res;
    int counter = 0;
    while(p)
    {
        if(counter == 0){
            p[0] = tolower(p[0]);
        }
        res = res + std::string(p);
        p=strtok(NULL,d);
        counter++;
    }
    return std::string(res);
}

std::string GetMethodName(Json::Value json_value) {
    // get href , from frontest to "{"
    const char * href = json_value["content"][0]["content"][0]["content"][1]["attributes"]["href"].asString().c_str();
    const char * action_type = json_value["content"][0]["content"][0]["content"][1]["content"][0]["content"][1]["content"][0]["attributes"]["method"].asString().c_str();
    std::string method_action_type = "";
    if(strcmp(action_type,"GET")){
        method_action_type = "get";
    }else if(strcmp(action_type,"PUT")){
        method_action_type = "edit";
    }else if(strcmp(action_type,"POST")){
        method_action_type = "add";
    }
    char *route;
    const char *div = "{";
    char route_array[100];
    strcpy(route_array,href);
    route = strtok(route_array, div);
    
    // get the last of directory as method name
    char* mehtod_name_underscore;
    const char *div_2 = "/";
    char mehtod_name_underscore_array[100];
    strcpy(mehtod_name_underscore_array,route);
    char *pointer;
    pointer = strtok(mehtod_name_underscore_array,div_2);
   
    while(pointer)
    {
        mehtod_name_underscore = pointer;
        pointer=strtok(NULL,div_2);
    }
    
    // change underscores to carmel case
    char mehtod_name_array[100];
    strcpy(mehtod_name_array,mehtod_name_underscore);
    
    const char *div_3 = "_";
    char *pointer_2;
    pointer_2 = strtok(mehtod_name_array,div_3);
    std::string res;
    char each_word_array[100];
    while(pointer_2)
    {
        strcpy(each_word_array, pointer_2);
            each_word_array[0] = toupper(each_word_array[0]);
        res = res + std::string(each_word_array);
        pointer_2=strtok(NULL,div_3);
    }
    res = method_action_type + res;
    return res ;
}

int main(int argc, const char * argv[]) {
    mdp::ByteBuffer blueprint = haze::FileSystem::readFile("/Users/huteng/api-cherry/cherry-boom/cherry-boom/test_data/test.apib");
    Json::Value value = ConvertBluePrintToJson(blueprint.c_str());
    std::string result2 = GetGroupName(value);

    std::string method_name = GetMethodName(value);
    ControllerMaker(result2, method_name);
    ServiceMaker(result2);
    RepositoryMaker(result2);
}


