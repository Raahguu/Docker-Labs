#include <iostream>
#include "docker_labs/core/docker_hook.h"
#include "docker_labs/core/curl_wrapper.h"

using namespace Docker_Labs;

Labs_Core::Container Labs_Core::Docker::Get_Container(std::string container_name)
{
	std::string url = "/containers/json?all=true&filters=%7B%22name%22%3A%5B%22^" + container_name + "$%22%5D%7D";
	json response = CallDockerAPI(url);

	if (response["httpCode"] == 404 || response["httpCode"] == 500) {
		std::cerr << "Error " << response["httpCode"] << " getting container " << container_name << ": " << response["body"]["message"] << std::endl;
		throw "Error";
	}

	if (response["body"] == json::array()) {
		url = "/containers/" + container_name + "/json";
		response = CallDockerAPI(url);

		if (response["httpCode"] == 404) {
			std::cerr << "There is no container with the name/id " << container_name << std::endl;
			throw response["httpCode"];
		}
		if (response["httpCode"] == 500) {
			std::cerr << "Internal server error 500: " << response["body"]["message"] << std::endl;
			throw response["httpCode"];
		}
	}

	std::string id;

	if (response["body"].is_array()) {
		id = response["body"][0]["Id"];
	} else {
		id = response["body"]["Id"];
	}


	Labs_Core::Container container = Labs_Core::Container(id);
	container.Cache_Update();
	return container;
}

Labs_Core::Container Labs_Core::Docker::Create_Container(std::string container_name, std::string image_name) {
	Labs_Core::Network network = Create_Network("network" + container_name);

	std::string url = "/containers/create?name=" + container_name;

	std::string request_data = R"({
			"Image": ")" + image_name + R"(",
			"Labels": {
				"Docker_Labs": "true"
			},
			"HostConfig": {
				"RestartPolicy": {
					"Name": "unless-stopped"
				}
			},
			"NetworkingConfig": {
				"EndpointsConfig": {
					")" + network.Get_Name_Cache() + R"(": {}			
				}			
			}
		})";

	json response = CallDockerAPI(url, request_data, "POST");

	int httpCode = response["httpCode"];

	if (httpCode != 201) {
		std::string error_message = "Error " + std::to_string(httpCode) + ": " + response["body"]["message"].dump();
		std::cout << error_message << std::endl;
		throw error_message;
	}
	std::cout << "Successfully created " << container_name << std::endl;

	Labs_Core::Container container = Labs_Core::Container(response["body"]["Id"]);
	container.Cache_Update();
	return container;
}

Labs_Core::Container Labs_Core::Docker::Reset(Labs_Core::Container container)
{
	
	std::string url = "/containers/" + Get_Container_ID(container) + "/json";
	json response = CallDockerAPI(url);

	if (response["httpCode"] == 404 || response["httpCode"] == 500) {
		std::cout << "Error " << response["httpCode"] << " getting name of container " << Get_Container_ID(container) << ": " << response["body"]["message"] << std::endl;
		throw "Error";
	}

	std::string container_name = response["body"]["Name"].dump().substr(2, response["body"]["Name"].dump().length() - 3);


	static const std::vector<std::string> ConfigKeys = {
		"Hostname", "Domainname", "User", "AttachStdin", "AttachStdout", "AttachStderr",
		"ExposedPorts", "Tty", "OpenStdin", "StdinOnce", "Env", "Cmd", "Healthcheck",
		"ArgsEscaped", "Image", "Volumes", "WorkingDir", "Entrypoint", "OnBuild", "Labels",
		"StopSignal", "StopTimeout", "Shell"
	};
	
	// Whitelisted keys from HostConfig
	static const std::vector<std::string> HostConfigKeys = {
		"Binds", "ContainerIDFile", "LogConfig", "NetworkMode", "PortBindings",
		"RestartPolicy", "AutoRemove", "VolumeDriver", "VolumesFrom", "CapAdd",
		"CapDrop", "CgroupnsMode", "Dns", "DnsOptions", "DnsSearch", "ExtraHosts",
		"GroupAdd", "IpcMode", "Cgroup", "Links", "OomScoreAdj", "PidMode",
		"Privileged", "PublishAllPorts", "ReadonlyRootfs", "SecurityOpt", "UTSMode",
		"UsernsMode", "ShmSize", "Runtime", "ConsoleSize", "CpuShares", "Memory",
		"NanoCpus", "CgroupParent", "BlkioWeight", "BlkioWeightDevice",
		"BlkioDeviceReadBps", "BlkioDeviceWriteBps", "BlkioDeviceReadIOps",
		"BlkioDeviceWriteIOps", "CpuPeriod", "CpuQuota", "CpuRealtimePeriod",
		"CpuRealtimeRuntime", "CpusetCpus", "CpusetMems", "Devices", "DeviceCgroupRules",
		"DeviceRequests", "MemoryReservation", "MemorySwap", "MemorySwappiness",
		"OomKillDisable", "PidsLimit", "Ulimits", "CpuCount", "CpuPercent",
		"IOMaximumIOps", "IOMaximumBandwidth", "MaskedPaths", "ReadonlyPaths"
	};
	
	auto copyAllowedKeys = [](const json& source, json& target, const std::vector<std::string>& allowedKeys) {
		for (const auto& key : allowedKeys) {
			if (source.contains(key)) {
				target[key] = source[key];
			}
		}
	};
	
	json createPayload = json::object();

	// Copy Config fields
	if (response["body"].contains("Config")) {
		copyAllowedKeys(response["body"]["Config"], createPayload, ConfigKeys);
	}
	
	// Copy HostConfig fields
	if (response["body"].contains("HostConfig")) {
		json hostConfigJson;
		copyAllowedKeys(response["body"]["HostConfig"], hostConfigJson, HostConfigKeys);
		createPayload["HostConfig"] = hostConfigJson;
	}

	
	// Build NetworkingConfig from NetworkSettings.Network
	if (response["body"].contains("NetworkSettings") && response["body"]["NetworkSettings"].contains("Networks")) {
		const json& networks = response["body"]["NetworkSettings"]["Networks"];
		if (networks.is_object()) {
			json endpointsConfig = json::object();
			for (auto it = networks.begin(); it != networks.end(); ++it){
				const std::string& networkName = it.key();
				const json& netInfo = it.value();
				json endpointConfig = json::object();
				json ipamConfig;
				
				if (!ipamConfig.empty()) {
					endpointConfig["IPAMConfig"] = ipamConfig;
				}
				
				if (netInfo.contains("Aliases") && !netInfo["Aliases"].is_null()) {
					endpointConfig["Aliases"] = netInfo["Aliases"];
				}

				if (netInfo.contains("MacAddress") && !netInfo["MacAddress"].get<std::string>().empty()) {
					endpointConfig["MacAddress"] = netInfo["MacAddress"];
				}
				
				endpointsConfig[networkName] = endpointConfig;
			}
			if (!endpointsConfig.empty()) {
				createPayload["NetworkingConfig"]["EndpointsConfig"] = endpointsConfig;
			}
		}
	}

	bool running = Get_Status(container);

	Remove(container, false);

	url = "/containers/create?name=" + container_name;
	response = CallDockerAPI(url, createPayload.dump(), "POST");

	if (response["httpCode"] != 201) {
		std::cout << "Error " << response["httpCode"] << " resesting container " << container_name << ": " << response["body"]["message"] << std::endl;
		throw "Error";
	}

	Labs_Core::Container new_container = Labs_Core::Container(response["body"]["Id"]);

	if (running) {
		Start(new_container);
	}

	new_container.Cache_Update();

	std::cout << "Container Successfully Reset" << std::endl;
	return new_container;
}

std::vector<Labs_Core::Container> Labs_Core::Docker::Get_All_Containers(){
	std::string url = "/containers/json?all=true";
	json response = CallDockerAPI(url);

	if (response["httpCode"] == 404 || response["httpCode"] == 500) {
		std::cerr << "Error " << response["httpCode"] << " getting containers: " << response["body"]["message"] << std::endl;
		throw "Error";
	}

	std::vector<Labs_Core::Container> containers;

	for(json con_json : response["body"]){
		if(!con_json["Labels"].contains("Docker_Labs") || con_json["Labels"]["Docker_Labs"] != "true") {
			continue;
		}
		Labs_Core::Container container = Labs_Core::Container(con_json["Id"]);
		container.Cache_Update();
		containers.push_back(container);
	}

	return containers;
}


std::string Labs_Core::Docker::Get_Container_ID(Labs_Core::Container container)
{
	return container.Get_ID();
}

std::string Labs_Core::Docker::Get_Container_Name(Labs_Core::Container container)
{
	std::string url = "/containers/" + Get_Container_ID(container) + "/json";
	json response = CallDockerAPI(url);

	if (response["httpCode"] == 404 || response["httpCode"] == 500) {
		std::cout << "Error " << response["httpCode"] << " getting name of container " << Get_Container_ID(container) << ": " << response["body"]["message"] << std::endl;
		return "";
	}

	return response["body"]["Name"].empty() ? Get_Container_ID(container) : response["body"]["Name"].dump().substr(2, response["body"]["Name"].dump().length() - 3);
}

std::string Labs_Core::Docker::Get_Image(Labs_Core::Container container)
{
	std::string url = "/containers/" + Get_Container_ID(container) + "/json";
	json response = CallDockerAPI(url);

	if (response["httpCode"] == 404 || response["httpCode"] == 500) {
		std::cerr << "Error " << response["httpCode"] << " getting image of container " << Get_Container_Name(container) << ": " << response["body"]["message"] << std::endl;
		throw "Error";
	}

	return response["body"]["Config"]["Image"];
}

std::string Labs_Core::Docker::Get_IP(Labs_Core::Container container)
{
	std::string url = "/containers/" + Get_Container_ID(container) + "/json";
	json response = CallDockerAPI(url);

	if (response["httpCode"] == 404 || response["httpCode"] == 500) {
		std::cerr << "Error " << response["httpCode"] << " getting IP of container " << Get_Container_Name(container) << ": " << response["body"]["message"] << std::endl;
		throw "Error";
	}

	try {
		for(auto& [Name, Data] : response["body"]["NetworkSettings"]["Networks"].items()) {
			return Data["IPAddress"];
		}
		return "";
	} catch (const nlohmann::json::type_error&) {
		return "";
	}
}

std::vector<Labs_Core::Network> Labs_Core::Docker::Get_Networks(Labs_Core::Container container)
{
	std::string url = "/containers/" + Get_Container_ID(container) + "/json";
	json response = CallDockerAPI(url);

	if (response["httpCode"] == 404 || response["httpCode"] == 500) {
		std::cerr << "Error " << response["httpCode"] << " getting networks of container " << Get_Container_Name(container) << ": " << response["body"]["message"] << std::endl;
		throw "Error";
	}

	std::vector<Labs_Core::Network> networks;

	const json names = response["body"]["NetworkSettings"]["Networks"];
	for (auto it = names.begin(); it != names.end(); ++it) {
		networks.push_back(Network(it.key()));
	}

	return networks;
}

bool Labs_Core::Docker::Get_Status(Labs_Core::Container container)
{
	std::string url = "/containers/" + Get_Container_ID(container) + "/json";
	json response = CallDockerAPI(url);

	if (response["httpCode"] == 404 || response["httpCode"] == 500) {
		std::cerr << "Error " << response["httpCode"] << " getting status of container " << Get_Container_Name(container) << ": " << response["body"]["message"] << std::endl;
		throw "Error";
	}

	return response["body"]["State"]["Running"];
}


int Labs_Core::Docker::Start(Labs_Core::Container container)
{
	std::string url = "/containers/" + Get_Container_ID(container) + "/start";
	json response = CallDockerAPI(url, "", "POST");

	if (response["httpCode"] == 404 || response["httpCode"] == 500) {
		std::cerr << "Error " << response["httpCode"] << " starting container " << Get_Container_Name(container) << ": " << response["body"]["message"] << std::endl;
		throw "Error";
	}
	if (response["httpCode"] == 304) {
		std::cout << "Container " << Get_Container_Name(container) << " was already up" << std::endl;
		return 304;
	}

	std::cout << "Container " << Get_Container_Name(container) << " successfully started" << std::endl;

	return 204;
}

int Labs_Core::Docker::Stop(Labs_Core::Container container)
{
	std::string url = "/containers/" + Get_Container_ID(container) + "/stop";
	json response = CallDockerAPI(url, "", "POST");

	if (response["httpCode"] == 404 || response["httpCode"] == 500) {
		std::cerr << "Error " << response["httpCode"] << " stopping container " << Get_Container_Name(container) << ": " << response["body"]["message"] << std::endl;
		throw "Error";
	}
	if (response["httpCode"] == 304) {
		std::cout << "Container " << Get_Container_Name(container) << " was already down" << std::endl;
		return 304;
	}

	std::cout << "Container " << Get_Container_Name(container) << " successfully stopped" << std::endl;

	return 204;
}

int Labs_Core::Docker::Restart(Labs_Core::Container container)
{
	std::string url = "/containers/" + Get_Container_ID(container) + "/restart";
	json response = CallDockerAPI(url, "", "POST");

	if (response["httpCode"] == 404 || response["httpCode"] == 500) {
		std::cerr << "Error " << response["httpCode"] << " restarting container " << Get_Container_Name(container) << ": " << response["body"]["message"] << std::endl;
		throw "Error";
	}

	std::cout << "Container " << Get_Container_Name(container) << " successfully restarted" << std::endl;

	return 204;
}

int Labs_Core::Docker::Kill(Labs_Core::Container container)
{
	std::string url = "/containers/" + Get_Container_ID(container) + "/kill";
	json response = CallDockerAPI(url, "", "POST");


	if (response["httpCode"] == 404 || response["httpCode"] == 500) {
		std::cout << "Error " << response["httpCode"] << " killing container " << Get_Container_Name(container) << ": " << response["body"]["message"] << std::endl;
		return response["httpCode"];
	}
	if (response["httpCode"] == 409) {
		std::cout << "Container " << Get_Container_Name(container) << " was already stopped" << std::endl;
		return 409;
	}

	std::cout << "Container " << Get_Container_Name(container) << " successfully stopped" << std::endl;

	return 204;
}

int Labs_Core::Docker::Remove(Labs_Core::Container container, bool remove_network)
{
	std::string name = Get_Container_Name(container);

	std::string url = "/containers/" + Get_Container_ID(container) + "?force=true";
	json response = CallDockerAPI(url, "", "DELETE");


	if (response["httpCode"] == 400 || response["httpCode"] == 404 || response["httpCode"] == 500) {
		std::cout << "Error " << response["httpCode"] << " deleting container " << name << ": " << response["body"]["message"] << std::endl;
		return response["httpCode"];
	}

	std::cout << "Container successfully removed" << std::endl;

	if (remove_network) { Delete_Network(Get_Network("network" + name));}

	return 204;
}

int Labs_Core::Docker::Remove(Labs_Core::Container container) {
	return Remove(container, true);
}

