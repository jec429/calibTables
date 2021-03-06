#include "TTPC_Wire_Channel_Table.hxx"
#include "TTPC_Wire_Geometry_Table.hxx"

#include <TResultSetHandle.hxx>
#include <DatabaseUtils.hxx>

#include <TEventContext.hxx>
#include <TCaptLog.hxx>
#include <TChannelId.hxx>

#include <iostream>
#include <string>

int main() {

    CP::TCaptLog::Configure();
    // CP::TCaptLog::SetDebugLevel(CP::TCaptLog::TraceLevel);
    
    // Examine the database tables 5 days between the start
    // and end date.

    std::string start_date_time = "2015-10-11 00:00:00";
    std::string end_date_time   = "2015-10-17 00:00:00";

    Int_t start_unix_time = DateTimeToUnixTime(start_date_time);
    Int_t end_unix_time   = DateTimeToUnixTime(end_date_time);

    for (Int_t current_unix_time = start_unix_time;
         current_unix_time <= end_unix_time;
         current_unix_time += 5*24*60*60) {

        CP::TEventContext context;
        context.SetTimeStamp(current_unix_time);
        context.SetPartition(CP::TEventContext::kmCAPTAIN);
        
        CaptLog(UnixTimeToDateTime(current_unix_time));

        // Get the channel table.
        CP::TResultSetHandle<CP::TTPC_Wire_Channel_Table> chanTable(context);
        Int_t numChannel(chanTable.GetNumRows());

        // Get the geometry table.
        CP::TResultSetHandle<CP::TTPC_Wire_Geometry_Table> geomTable(context);
        Int_t numGeometry(geomTable.GetNumRows());
        if (numChannel == 0 || numGeometry == 0) {
            CaptError("Problem with query " << numChannel
                      << " " << numGeometry
                      << " " << context);
            continue;
        }
        CaptLog("Query at " 
                << UnixTimeToDateTime(current_unix_time) 
                << " ... result set contains "
                << numChannel << " channels"
                << " and " << numGeometry << " wires");
        for (int i = 0; i<numChannel; ++i) {
            const CP::TTPC_Wire_Channel_Table* row = chanTable.GetRow(i);
            if (!row) {
                CaptError("Missing channel row " << i);
                continue;
            }
            // row->Print();
            int wire = row->GetWire();
            const CP::TTPC_Wire_Geometry_Table* geomRow 
                = geomTable.GetRowByIndex(wire);
            if (!geomRow && wire>0) {
                CaptError("Missing geometry row " << wire);
            }
            if (geomRow) geomRow->Print();
        }
    }

    return 0;
}
