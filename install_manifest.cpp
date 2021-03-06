/** \file install_manifest.cpp
 *
 * Created: JohnE, 2008-07-11
 */


/*
DISCLAIMER:
The author(s) of this file's contents release it into the public domain, without
express or implied warranty of any kind. You may use, modify, and redistribute
this file freely.
*/


#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "install_manifest.hpp"

#include <cstring>
#include <list>


InstallManifest::InstallManifest(const StringType& loadfile)
 : cur_comp(entry_setmap.end())
{
	if (doc.LoadFile(loadfile.c_str()) == tinyxml2::XML_SUCCESS)
	{
		std::list< tinyxml2::XMLElement* > search_els;
		search_els.push_back(doc.RootElement());
		while (!search_els.empty())
		{
			tinyxml2::XMLElement* el = search_els.front();
			search_els.pop_front();
			tinyxml2::XMLElement* ent = el->FirstChildElement("Entry");
			if (ent)
			{
				const char* cid = el->Attribute("id");
				if (cid && strlen(cid) > 0)
				{
					SetComponent(cid);
					for (; ent; ent = ent->NextSiblingElement("Entry"))
						AddEntry(ent->FirstChild()->ToText()->Value());
				}
			}
			else
			{
				for (tinyxml2::XMLElement* child = el->FirstChildElement();
				 child;
				 child = child->NextSiblingElement())
					search_els.push_back(child);
			}
		}
	}
}


const tinyxml2::XMLElement* InstallManifest::GetComponent(const StringType& comp_id) const
{
	IDToEntrySet_Map::const_iterator found = entry_setmap.find(comp_id);
	return (found == entry_setmap.end()) ? 0 : found->second.first;
}


tinyxml2::XMLElement* InstallManifest::SetComponent(const StringType& comp_id)
{
	IDToEntrySet_Map::iterator found = entry_setmap.find(comp_id);
	if (found != entry_setmap.end())
	{
		cur_comp = found;
		return found->second.first;
	}
	else
	{
		tinyxml2::XMLElement* new_comp = doc.NewElement("Component");
		new_comp->SetAttribute("id", comp_id.c_str());
		cur_comp = entry_setmap.insert(
		 std::make_pair(
		  std::string(comp_id),
		  std::make_pair(new_comp, std::set< std::string >())
		  )
		 ).first;
		return new_comp;
	}
}


void InstallManifest::AddEntry(const char* entry)
{
	if (cur_comp == entry_setmap.end())
		return;
	if (cur_comp->second.second.insert(entry).second)
	{
		tinyxml2::XMLElement* ent_el = doc.NewElement("Entry");
		tinyxml2::XMLText* ent_txt = doc.NewText(entry);
		ent_txt->SetCData(true);
		ent_el->LinkEndChild(ent_txt);
		cur_comp->second.first->LinkEndChild(ent_el);
		EntryToRefCount_Map::iterator inserted = entry_ref_counts.insert({entry, 0}).first;
		++(inserted->second);
	}
}


size_t InstallManifest::DecrementRemove(const char* entry)
{
	EntryToRefCount_Map::iterator found = entry_ref_counts.end();
	if (entry)
		found = entry_ref_counts.find(entry);
	if (found != entry_ref_counts.end())
	{
		if (found->second > 0)
			--(found->second);
		return found->second;
	}
	return 0;
}
