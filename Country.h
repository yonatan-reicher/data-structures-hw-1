#ifndef COUNTRY_H
#define COUNTRY_H

class Country {
public:
    int m_id;
    int m_numOfMedals;
    int m_numOfTeams;
    int m_numOfContestants;

    Country(int id, int numOfMedals);
    Country(const Country& other) = delete;
    Country(Country&& other) = delete;

    bool canBeDeleted() const;
};

#endif
