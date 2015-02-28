#include <iostream>
#include <cstdlib>
#include <cstring>
#include <stdlib.h>

#include "familytree.h"
#include "PersonWrapper.h"
#include "hw3/HashTable.h"
#include "hw3/StackAr.h"

using namespace std;

FamilyTree::FamilyTree(Family *families, int familyCount)
{
	PersonWrapper pw;
	pw.uid = -1;

	edges = new QuadraticHashTable<PersonWrapper> (pw, familyCount * 7);

	for(int i = 0; i < familyCount; i++) {
		Family family = families[i];
		Person null_parent;
		null_parent.firstName[0] = '\0';
		null_parent.lastName[0] = '\0';
		null_parent.year = 0;

		pw.uid = createUid(family.person);
		if(edges->find(family.person).uid <=  0) {
			pw.person = family.person;
			pw.parent[0] = null_parent;
			pw.parent[1] = null_parent;
			edges->insert(pw);

			people++;
		}

		pw.uid = createUid(family.spouse);
		if(edges->find(family.person).uid <= 0) {
			pw.person = family.spouse;
			pw.parent[0] = null_parent;
			pw.parent[1] = null_parent;
			edges->insert( pw);

			people++;
		}

		for(int n = 0; n < family.childCount; n++) {
			pw.uid = createUid(family.children[n]);
			pw.person = family.children[n];
			pw.parent[0] = family.person;
			pw.parent[1] = family.spouse;
			edges->insert( pw);

			people++;
		}
	}
} // FamilyTree()


/* =================
 *   Run queries
 * =================
 */

void FamilyTree::runQueries(Query *queries, Person *answers, int queryCount)
{

	PersonWrapper pw_tmp;
	pw_tmp.person.year = -1;
	
	for(int i = 0; i < queryCount; i++) {

		QuadraticHashTable<PersonWrapper> *ht = new QuadraticHashTable<PersonWrapper>(pw_tmp, 5000);
		PersonWrapper pw = edges->find(queries[i].person1);
		
		// Get ancestors for person 1
		QuadraticHashTable<PersonWrapper> *ansc1 = extractAncestors(pw, ht);
		
		// Find youngest ancestor
		pw = edges->find(queries[i].person2);
		pw_tmp.person.year = -1;
		answers[i] = findYoungest(ansc1, pw, pw_tmp, 0).person;

		delete(ht);
	}

}  // runQueries()

/* =========================
 *   Generate unique id
 * =========================
 */

int FamilyTree::createUid(Person p) {
	/*
	int uid = -1;

	memcpy(&uid, p.firstName, 4);
	uid = uid + p.lastName[0] * 7+ p.lastName[1] + p.lastName[2] * 7 + p.lastName[3]; 
	uid = uid + p.gender * 13;
	uid = uid + p.year * 7;
	
	if(uid < 0) uid = -uid;
	return uid;
	*/

						int seed = 131;
						int uid = 0;
						
          	for(int i = 0; i < strlen(p.lastName); i++) {
          		uid = (uid * seed) + p.lastName[i];
          	}
						
						for(int i = 0; i < strlen(p.firstName); i++) {
          		uid = (uid * seed) + p.firstName[i];
          	}
						
          	uid = uid + p.year * 37;
          	uid = uid + p.gender * seed;


            if( uid < 0 ) uid = -uid;

            return uid ;
}

QuadraticHashTable<PersonWrapper>* FamilyTree::extractAncestors(PersonWrapper pw, QuadraticHashTable<PersonWrapper> *ht) {
	if(pw.uid <= 0) return ht;

	PersonWrapper p1 = edges->find(pw.parent[0]);
	PersonWrapper p2 = edges->find(pw.parent[1]);

//cout << pw.person.firstName << " " << pw.person.lastName << " " << pw.uid << endl;
//cout << "Hi my name is " << pw.person.firstName << " (" << pw.uid <<") and my parents are " <<
//	pw.parent[0].firstName << " ("<< edges->find(pw.parent[0]).uid<< ") and " << pw.parent[1].firstName << endl;


	ht->insert( pw);
	extractAncestors(p1, ht);
	extractAncestors(p2, ht);

	return ht;
}

/*
QuadraticHashTable<PersonWrapper>* FamilyTree::extractAncestors(PersonWrapper pw, QuadraticHashTable<PersonWrapper> *ht) {
	StackAr<PersonWrapper> s(100000);
	PersonWrapper p1 = edges->find(pw.parent[0]);
	PersonWrapper p2 = edges->find(pw.parent[1]);
	s.push(p2);

	while(!s.isEmpty()) {
		if(pw.uid >= 0)  {
			ht->insert(pw);
			pw = p1;
			s.push(p2);
		} else {
			pw = s.top();
			//s.pop();
		}
	}

	return ht;
}
*/

PersonWrapper FamilyTree::findYoungest(QuadraticHashTable<PersonWrapper>* ht, PersonWrapper pw, PersonWrapper &youngest, int count) {
	if(pw.uid <= 0) return youngest;
	if(ht->find(pw.person).uid)  {
		if(pw.person.year > youngest.person.year) youngest = pw;
	}

	PersonWrapper p1 = edges->find(pw.parent[0]);
	PersonWrapper p2 = edges->find(pw.parent[1]);

	findYoungest(ht, p1, youngest, count);
	findYoungest(ht, p2, youngest, count);
	
	return youngest;
}