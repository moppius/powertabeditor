/*
  * Copyright (C) 2014 Cameron White
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "repeatindexer.h"

#include <score/score.h>
#include <score/utils.h>
#include <stack>

RepeatedSection::RepeatedSection(const SystemLocation &startBar)
    : myStartBarLocation(startBar), myActiveRepeat(1)
{
}

bool RepeatedSection::operator<(const RepeatedSection &other) const
{
    return myStartBarLocation < other.myStartBarLocation;
}

void RepeatedSection::addRepeatEndBar(const SystemLocation &location,
                                      int repeatCount)
{
    myRepeatEndBars[location] = repeatCount;
    myRemainingRepeats[location] = repeatCount - 1;
}

void RepeatedSection::addAlternateEnding(int system,
                                         const AlternateEnding &ending)
{
    const SystemLocation location(system, ending.getPosition());

    // For each repeat that the ending is active, record the position
    // that should be jumped to.
    for (int num : ending.getNumbers())
        myAlternateEndings[num] = location;
}

const SystemLocation &RepeatedSection::getStartBarLocation() const
{
    return myStartBarLocation;
}

const SystemLocation &RepeatedSection::getLastEndBarLocation() const
{
    assert(!myRepeatEndBars.empty());
    return myRepeatEndBars.rbegin()->first;
}

int RepeatedSection::getAlternateEndingCount() const
{
    return static_cast<int>(myAlternateEndings.size());
}

int RepeatedSection::getTotalRepeatCount() const
{
    int count = 0;

    for (auto &bar : myRepeatEndBars)
        count += bar.second;

    return count;
}

boost::optional<SystemLocation> RepeatedSection::findAlternateEnding(int number) const
{
    if (myAlternateEndings.find(number) != myAlternateEndings.end())
        return myAlternateEndings.find(number)->second;
    else
        return boost::optional<SystemLocation>();
}

void RepeatedSection::reset()
{
    myActiveRepeat = 1;

    // Reset the number of remaining repeats to the original values.
    for (auto &repeat : myRepeatEndBars)
        myRemainingRepeats[repeat.first] = repeat.second - 1;
}

SystemLocation RepeatedSection::performRepeat(const SystemLocation &loc)
{
    // Deal with alternate endings - if we are at the start of the first
    // alternate ending, we can branch off to other alternate endings depending
    // on the active repeat.
    boost::optional<SystemLocation> firstAltEnding = findAlternateEnding(1);
    if (firstAltEnding && *firstAltEnding == loc)
    {
        // Branch off to the next alternate ending, if it exists.
        boost::optional<SystemLocation> nextAltEnding =
            findAlternateEnding(myActiveRepeat);
        if (nextAltEnding)
            return *nextAltEnding;
    }

    // Now, we can look for repeat end bars.
    auto remainingRepeatCount = myRemainingRepeats.find(loc);

    // No repeat bar.
    if (remainingRepeatCount == myRemainingRepeats.end())
        return loc;
    // Perform the repeat event.
    else if (remainingRepeatCount->second != 0)
    {
        --remainingRepeatCount->second;
        ++myActiveRepeat;
        return myStartBarLocation;
    }
    // Otherwise, the repeat is not performed and is reset.
    else
    {
        remainingRepeatCount->second =
            myRepeatEndBars.at(remainingRepeatCount->first) - 1;
        return loc;
    }
}

RepeatIndexer::RepeatIndexer(const Score &score)
{
    // There may be nested repeats, so maintain a stack of the active repeats
    // as we go through the score.
    std::stack<RepeatedSection> repeats;

    // The start of the score can always act as a repeat start bar.
    repeats.push(SystemLocation(0, 0));

    int systemIndex = 0;
    for (const System &system : score.getSystems())
    {
        for (const Barline &bar : system.getBarlines())
        {
            // If we've seen the last alternate ending of the repeat,
            // we are done.
            if (!repeats.empty())
            {
                // TODO - currently, this allows a greater number of alternate
                // endings than the total repeat count so that repeats aren't
                // silently dropped. This should be reported
                // by a score checker-type feature when that is implemented.
                RepeatedSection &activeRepeat = repeats.top();
                if (activeRepeat.getAlternateEndingCount() &&
                    activeRepeat.getTotalRepeatCount() &&
                    activeRepeat.getAlternateEndingCount() >=
                        activeRepeat.getTotalRepeatCount())
                {
                    myRepeats.insert(activeRepeat);
                    repeats.pop();
                }
            }

            // Record any start bars that we see.
            if (bar.getBarType() == Barline::RepeatStart)
            {
                const SystemLocation location(systemIndex, bar.getPosition());
                repeats.push(RepeatedSection(location));
            }
            // TODO - report unexpected repeat end bars.
            else if (bar.getBarType() == Barline::RepeatEnd && !repeats.empty())
            {
                // Add this end bar to the active section.
                RepeatedSection &activeRepeat = repeats.top();
                activeRepeat.addRepeatEndBar(
                    SystemLocation(systemIndex, bar.getPosition()),
                    bar.getRepeatCount());

                // If we don't have any alternate endings, we must be
                // done with this repeat.
                if (activeRepeat.getAlternateEndingCount() == 0)
                {
                    myRepeats.insert(activeRepeat);
                    repeats.pop();
                }
            }

            // Process repeat endings in this bar, unless we're at the end bar.
            const Barline *nextBar = system.getNextBarline(bar.getPosition());
            if (nextBar)
            {
                for (const AlternateEnding &ending : ScoreUtils::findInRange(
                         system.getAlternateEndings(), bar.getPosition(),
                         nextBar->getPosition() - 1))
                {
                    // TODO - report unexpected alternate endings.
                    if (!repeats.empty())
                        repeats.top().addAlternateEnding(systemIndex, ending);
                }
            }
        }

        ++systemIndex;
    }

    // TODO - report mismatched repeat start bars.
    // TODO - report missing / extra alternate endings.
}

const RepeatedSection *RepeatIndexer::findRepeat(
    const SystemLocation &loc) const
{
    auto repeat = myRepeats.upper_bound(loc);

    // Search for a pair of start and end bars that surrounds this location.
    while (repeat != myRepeats.begin())
    {
        --repeat;
        if (repeat->getLastEndBarLocation() >= loc)
            return &(*repeat);
    }

    return nullptr;
}

RepeatedSection *RepeatIndexer::findRepeat(
    const SystemLocation &loc)
{
    return const_cast<RepeatedSection *>(
        const_cast<const RepeatIndexer *>(this)->findRepeat(loc));
}

boost::iterator_range<RepeatIndexer::RepeatedSectionIterator>
RepeatIndexer::getRepeats() const
{
    return boost::make_iterator_range(myRepeats);
}
