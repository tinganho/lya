
#ifndef SERVER_H
#define SERVER_H

#include <service.grpc.pb.h>
#include <service.pb.h>
#include <string>
#include <types.h>

using namespace std;
using namespace grpc;
using namespace Lya::protocol_buffers;
using namespace Lya::types;

namespace Lya::services {

	typedef function<tuple<vector<Localization>, vector<Diagnostic>>(const string&, const vector<string>&, uint64_t start_line)> ExtractLocalization;

	class ExtensionServer : public protocol_buffers::LyaService::Service {
	public:
	    ExtensionServer(
	        string _server_address,
	        ExtractLocalization _extract_localizations);
	    void start_server(bool quiet);
	    Status extract(ServerContext *context, const PBSyncRequest *request, PBSyncResponse *response) override;
		Status compile(ServerContext* context, const PBCompileRequest* request, PBCompileResponse* response) override;
	    Status check_availability(ServerContext* context, const PBAvailabilityRequest* request, PBAvailabilityResponse* response) override;

	private:
	    string server_address;
	    ExtractLocalization extract_localizations;
	};

} // Lya::Extension

#endif // SERVER_H
