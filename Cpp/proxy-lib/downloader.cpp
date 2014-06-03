#include <fost/aws>
#include <fost/insert>
#include <fost/timer>
#include <proxy/config.hpp>
#include <varanus/acanthurus.hpp>
#include <varanus/obor.hpp>


using namespace fostlib;


namespace {


    json s3_into_local_db(const json &config) {
        json result;
        aws::s3::bucket bucket(
            coerce<ascii_printable_string>(
                coerce<nullable<string>>(config["bucket"])
                    .value(proxy::c_bucket.value())));

        timer taken;
        string content(bucket.get(
            coerce<boost::filesystem::wpath>(
                coerce<nullable<string>>(config["location"])
                    .value("/configuration.json"))));
        json db_content(json::parse(content));
        insert(result, "downloaded", db_content);
        insert(result, "time-taken", taken.elapsed());

        varanus::db localdb(varanus::local_data(
            coerce<nullable<string>>(config["db"]).value("configuration"),
            db_content));
        fostlib::jsondb::local localdata(localdb());
        if ( localdata[jcursor()] != db_content ) {
            insert(result, "refreshed", true);
            localdata.set(jcursor(), db_content);
            localdata.commit();
        } else {
            insert(result, "refreshed", false);
        }
        return result;
    }
    const varanus::obor c_statistics(
        "proxy.downloader.db.local", s3_into_local_db);


}

