#include <kiwi.fast.plugin_utility/ptree_item.h>

#include <kiwi.fast.plugin_utility/detail/config.h>
#include <kiwi.fast.plugin_utility/type_converter.h>

#include <kiwi.fast.plugin_utility/ptree_visitor.h>

KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE

base_ptree_visitor* base_ptree_item::item(const char8_t* name)
{
	if (m_visitor == nullptr)
	{
		m_visitor = new ptree_visitor(this);
	}
	m_visitor->set_name(name);
	return m_visitor;
}

base_ptree_item::~base_ptree_item()
{
	delete m_visitor;
}

KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE