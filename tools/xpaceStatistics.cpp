#include <boost/program_options.hpp>
#include <iostream>
#include <iomanip>
#include <string>
#include <filesystem>
#include <fstream>
#include "nlohmann/json.hpp"
#include "ctml.hpp"
#include "log_file.hpp"
#include "statistic.hpp"

constexpr double FILTER_WIDTH_IN_SECONDS = 0.5;

void printDescription()
{
	std::cout
		<< "xpaceStatistics calculates various statistical properties of log-files acquired during motion tracking."
		<< std::endl;
	std::cout << std::endl << "Usage: xpaceStatistics [opts] xpace_XXX.log [more log-files]" << std::endl;
}

nlohmann::json createJSONStatistics(xpace::XPaceStatistic &statistic)
{
	using Key = xpace::XPaceStatistic::StatKey;
	nlohmann::json o;
	o["Mean Euclidean Distance"] = statistic[Key::MeanDistance];
	o["Min Euclidean Distance"] = statistic[Key::MinDistance];
	o["Max Euclidean Distance"] = statistic[Key::MaxDistance];
	o["Euclidean Distance StdDev"] = statistic[Key::StandardDeviation];
	o["Weighted Mean Euclidean Distance"] = statistic[Key::WeightedMeanDistance];
	o["Integrated Speed"] = statistic[Key::IntegratedSpeed];
	o["Weighted Integrated Speed"] = statistic[Key::PartitionWeightedIntegratedSpeed];
	return o;
}

nlohmann::json createJSONMotions(const xpace::XPaceStatistic &statistic)
{
	nlohmann::json result;
	auto times = statistic.getTimes();
	auto trans = statistic.getTranslations();
	auto rots = statistic.getRotations();
	const auto &speed = statistic.getSpeed(0.0);

	for (int i = 0; i < times.size(); ++i) {
		nlohmann::json o;
		o["time"] = times[i];
		o["x"] = trans[i][0];
		o["y"] = trans[i][1];
		o["z"] = trans[i][2];
		o["roll"] = rots[i][0];
		o["pitch"] = rots[i][1];
		o["yaw"] = rots[i][2];
		o["speed"] = speed[i];
		result.emplace_back(o);
	}
	return result;
}

nlohmann::json createJSONMotionsForViz(const xpace::XPaceStatistic &statistic)
{
	nlohmann::json result;
	auto times = statistic.getTimes();

	auto span = times.back() - times.front();
	auto dt = span / times.size();
	auto sigma = FILTER_WIDTH_IN_SECONDS / dt;
	auto timeStep = FILTER_WIDTH_IN_SECONDS / 2.0;

	auto transX = statistic.getTranslationXFiltered(sigma);
	auto transY = statistic.getTranslationYFiltered(sigma);
	auto transZ = statistic.getTranslationZFiltered(sigma);
	auto rotRoll = statistic.getRotationRollFiltered(sigma);
	auto rotPitch = statistic.getRotationPitchFiltered(sigma);
	auto rotYaw = statistic.getRotationYawFiltered(sigma);
	const auto &speed = statistic.getSpeed(sigma);

	nlohmann::json o;

	double currentTime = 0.0;
	for (int i = 0; i < times.size(); ++i) {
		if (currentTime < times[i]) {
			currentTime += timeStep;
			o["time"].emplace_back(times[i]);
			o["dx"].emplace_back(transX[i]);
			o["dy"].emplace_back(transY[i]);
			o["dz"].emplace_back(transZ[i]);
			o["roll"].emplace_back(rotRoll[i]);
			o["pitch"].emplace_back(rotPitch[i]);
			o["yaw"].emplace_back(rotYaw[i]);
			o["speed"].emplace_back(speed[i]);
		}
	}
	return o;
}

std::string createHTMLTemplate(const std::string &filename, const nlohmann::json &stats, const nlohmann::json &motion)
{
	using namespace CTML;
	Document doc;
	doc.AppendNodeToHead(Node("title", filename));
	doc.AppendNodeToHead(
		Node("link")
			.SetAttribute("rel", "stylesheet")
			.SetAttribute("href", "https://cdnjs.cloudflare.com/ajax/libs/mini.css/3.0.1/mini-default.min.css")
	);
	doc.AppendNodeToHead(Node("script").SetAttribute("src", "https://cdn.jsdelivr.net/npm/vega@5.17.0"));
	doc.AppendNodeToHead(Node("script").SetAttribute("src", "https://cdn.jsdelivr.net/npm/vega-lite@4.17.0"));
	doc.AppendNodeToHead(Node("script").SetAttribute("src", "https://cdn.jsdelivr.net/npm/vega-embed@6.12.2"));
	doc.AppendNodeToHead(Node("style", ":root {--card-back-color: #fff;} table:not(.horizontal) {max-height: 500px}"));

	doc.AppendNodeToBody(Node("header.sticky").AppendChild(Node("h2", filename)));

	auto statsTable = Node("table.hoverable").AppendChild(
		Node("thead").AppendChild(Node("tr").AppendChild(Node("th", "Measure")).AppendChild(Node("th", "Value")))
	);

	auto tbody = Node("tbody");
	for (const auto &value: stats.items()) {
		tbody.AppendChild(
			Node("tr")
				.AppendChild(Node("td", value.key()))
				.AppendChild(Node("td", value.value().dump()))
		);
	}

	statsTable.AppendChild(tbody);

	auto statsCard = Node("div").SetAttribute("class", "card fluid")
		.AppendChild(Node("h2", "Statistical Measures").SetAttribute("class", "section double-padded"))
		.AppendChild(Node("div.section").AppendChild(statsTable));

	auto graphCard = Node("div").SetAttribute("class", "card fluid")
		.AppendChild(Node("h2", "Motion Graphs").SetAttribute("class", "section double-padded"))
		.AppendChild(Node("div.section#vis"));

	doc.AppendNodeToBody(
		Node("div.container")
			.AppendChild(Node("div.row")
							 .AppendChild(
								 Node("main").SetAttribute("class", "col-sm-12 col-md-8 col-lg-9")
									 .AppendChild(statsCard)
									 .AppendChild(graphCard)))
	);

	nlohmann::json vegaConfig = R"(
{
  "$schema": "https://vega.github.io/schema/vega-lite/v4.json",
  "config": {
    "axis": {
      "titleFontSize": 12,
      "labelFontSize": 12,
      "title": null
    },
    "legend": {
      "titleFontSize": 16,
      "labelFontSize": 16
    },
    "title": {
      "fontSize": 18
    },
    "range": {
      "category": [
        "#AD2B4E",
        "#2BAD3D",
        "#2B49AD"
      ]
    }
  },
  "description": "A simple bar chart with embedded data.",
  "vconcat": [
    {
      "title": "Movement",
      "width": 768,
      "mark": "line",
      "encoding": {
        "x": {
          "field": "time",
          "scale": {
            "domain": {
              "selection": "brush"
            }
          },
          "type": "quantitative",
          "axis": {
            "tickCount": 10
          }
        },
        "y": {
          "field": "translationValue",
          "type": "quantitative",
          "title": "[mm]"
        },
        "color": {
          "field": "translation",
          "type": "nominal",
          "title": "Legend"
        }
      }
    },
    {
      "title": "Rotation",
      "width": 768,
      "mark": "line",
      "encoding": {
        "x": {
          "field": "time",
          "scale": {
            "domain": {
              "selection": "brush"
            }
          },
          "type": "quantitative",
          "axis": {
            "tickCount": 10
          }
        },
        "y": {
          "field": "rotationValue",
          "type": "quantitative",
          "title": "[degree]"
        },
        "color": {
          "field": "rotation",
          "type": "nominal"
        }
      }
    },
    {
      "title": "Absolute Speed",
      "encoding": {
        "x": {
          "axis": {
            "title": "Time [s]",
            "tickCount": 10
          },
          "field": "time",
          "type": "quantitative"
        },
        "y": {
          "field": "speed",
          "type": "quantitative",
          "title": "[mm/s]"
        },
        "color": {
          "value": "#AD2B4E"
        }
      },
      "mark": "line",
      "selection": {
        "brush": {
          "encodings": [
            "x"
          ],
          "type": "interval"
        }
      },
      "width": 768
    }
  ],
  "data": {
    "values": []
  },
  "transform": [
    {
      "flatten": [
        "time",
        "dx",
        "dy",
        "dz",
        "roll",
        "pitch",
        "yaw",
        "speed"
      ]
    },
    {
      "fold": [
        "dx",
        "dy",
        "dz"
      ],
      "as": [
        "translation",
        "translationValue"
      ]
    },
    {
      "fold": [
        "roll",
        "pitch",
        "yaw"
      ],
      "as": [
        "rotation",
        "rotationValue"
      ]
    }
  ]
}
	)"_json;
	vegaConfig["data"]["values"] = motion;
	std::stringstream vegaCode;
	vegaCode << "var spec = " << vegaConfig << "; vegaEmbed('#vis', spec);";
	Node vega("script", vegaCode.str());
	vega.SetAttribute("type", "text/javascript");
	doc.AppendNodeToBody(vega);
	return doc.ToString();
}

int main(int argc, char **argv)
{
	constexpr int SUCCESS = 0;
	constexpr int ERROR = -1;
	const std::string programName("xpaceStatistics");


	using namespace xpace;
	namespace po = boost::program_options;

	std::vector<std::string> inputFiles;
	po::variables_map vm;
	po::options_description opts_description("Options");
	opts_description.add_options()
		("help,h", "Shows this help message")

		("input,i",
		 po::value<std::vector<std::string> >(&inputFiles)->required(),
		 "Input XPace log-file. Note that log-files can also be specified by giving them as last arguments to xpaceStatistics.")

		("print,p",
		 "Print the statistics rather than writing output to JSON files. This will only print statistics, not motions.");
	po::positional_options_description opts_positional;
	opts_positional.add("input", -1);
	try {
		auto parsed = po::command_line_parser(argc, argv)
			.options(opts_description)
			.positional(opts_positional)
			.run();
		po::store(parsed, vm);

		if (vm.count("help")) {
			printDescription();
			std::cout << opts_description << std::endl;
			return SUCCESS;
		}

		po::notify(vm);

		// Read the log-files and calculate the statistics
		if (vm.count("input")) {
			using namespace std::filesystem;
			nlohmann::json stats, motions;
			for (const auto &f: inputFiles) {
				using Key = xpace::XPaceStatistic::StatKey;
				if (!exists(path(f)) || !is_regular_file(path(f))) {
					std::cerr << "File does not exist " << path(f);
					continue;
				}
				try {
					xpace::XpaceLogFile logFile(f);
					xpace::XPaceStatistic statistic(logFile);
					stats[f] = createJSONStatistics(statistic);
					motions[f]["raw"] = createJSONMotions(statistic);
					motions[f]["filtered"] = createJSONMotionsForViz(statistic);
				}
				catch (...) {
					std::cerr << "Could not open or read file " << path(f);
					continue;
				}

			}

			// Output either the statistics (only) to console or both statistics and motions to JSON files
			if (vm.count("print")) {
				std::cout << std::setw(4) << stats << std::endl << std::endl;
			}
			else {
				for (const auto &f: inputFiles) {
					auto parentPath = path(f).parent_path();
					auto fileName = path(f).stem();
					auto motionFile = fileName;
					motionFile += "_motion.json";
					auto statsFile = fileName;
					statsFile += "_stats.json";
					auto htmlFile = fileName;
					htmlFile += ".html";

					std::ofstream statsFS(statsFile);
					if (statsFS) {
						statsFS << std::setw(2) << stats[f];
						statsFS.close();
					}

					std::ofstream motionFS(motionFile);
					if (motionFS) {
						motionFS << std::setw(2) << motions[f]["raw"];
						motionFS.close();
					}

					std::ofstream htmlFS(htmlFile);
					if (htmlFS) {
						htmlFS << createHTMLTemplate(fileName, stats[f], motions[f]["filtered"]);
						htmlFS.close();
					}
				}
			}
			return SUCCESS;
		}
	}
	catch (po::required_option &e) {
		std::cerr << "Error: No input log-file specified. Try xpaceStatistics -h" << std::endl;
		return ERROR;
	}
	catch (po::unknown_option &e) {
		std::cerr << programName << ": illegal option " << e.get_option_name() << std::endl;
		return ERROR;
	}
	catch (std::runtime_error &e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}
	return SUCCESS;
}
