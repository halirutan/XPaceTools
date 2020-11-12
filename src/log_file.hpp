#pragma once
#include "math.hpp"

#include <string>
#include <regex>


namespace xpace
{

class XpaceLogFile
{
public:
	explicit XpaceLogFile(std::string fileName);
	std::string getFilename();
	[[nodiscard]] InitialPosition getInitialPosition() const;

	/**
	 * Returns the number of subject motions that were tracked by
	 * the PMC system. For a normal scan this can be > 50k entries.
	 * @return Number of tracked motions
	 */
	[[nodiscard]] size_t getNumberOfMotions() const;

	/**
	 * Returns the absolute position of the mouth-piece in scanner
	 * coordinates for each tracked motion of the PMC system.
	 * @return Absolute positions of tracked motions
	 */
	[[nodiscard]] std::vector<Motion> getAbsolutePositions() const;

	/**
	 * Returns the relative motions as they were tracked by the PMC
	 * system. Note that these values represent the deviations of the
	 * mouth-piece from its initial position.
	 * @see getInitialPosition()
	 * @return Tracked relative motions
	 */
	[[nodiscard]] std::vector<Motion> getMotions() const;


private:
	std::string fileName_;
	std::vector<Motion> motions_;
	std::vector<Motion> positions_;
	InitialPosition initialPose_;
	bool isXpaceLogFile();
	void parse();
	void calculateAbsolutePositions();
};

}
