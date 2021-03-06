#pragma once

#include <kiwi.fast.utility/detail/config.h>

#include <kiwi.fast.plugin_utility/code_conversion.h>

#include <boost/program_options.hpp>

KIWI_FAST_OPEN_UTILITY_NAMESPACE

/*!
 * \brief The command_line class
 * 借助boost::program_options管理命令行参数，但并不用其配置文件功能
 */
class service_command_line_imp
{
public:
    void add_option(std::u8string const& name, std::u8string const& description)
    {
        m_options_description.add_options()
                (KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(name).c_str()
                 , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(description).c_str());
    }

    void add_int_option(std::u8string const& name, std::u8string const& description)
    {
        m_options_description.add_options()
                (KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(name).c_str()
                 , boost::program_options::wvalue<int>()
                 , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(description).c_str());
    }

    /* 注意设置std::wstring类型选项的默认值时只能使用带两个参数的default_value函数 */

    //...


    void run(int argc, wchar_t* argv[], wchar_t* envp[])
    {
        do_run(argc, argv, envp);
    }

    void run(int argc, wchar_t* argv[])
    {
        do_run(argc, argv);
    }

    void output_options_description()
    {
        std::cout << m_options_description << '\n';
    }

    template<typename CharType>
    std::size_t variable_count(CharType const* name)
    {
        return variable_count<CharType>(std::basic_string<CharType>(name));
    }

    template<typename CharType>
    std::size_t variable_count(std::basic_string<CharType> const& name)
    {
        return m_variables_map.count(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(name));
    }

    template<typename CharType, typename T>
    std::optional<T> variable(CharType const* name)
    {
        return variable<CharType, T>(std::basic_string<CharType>(name));
    }

    template<typename CharType, typename T>
    std::optional<T> variable(std::basic_string<CharType> const& name)
    {
        if(variable_count(name))
        {
            return m_variables_map.at(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(name)).template as<T>();
        }
        else
        {
            return {};
        }
    }

protected:
    service_command_line_imp()
        :m_options_description(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"所有选项"), 500)  //注意这里写死是500，所以描述信息不要长
    {

    }

    virtual ~service_command_line_imp(){}

    void do_run(int argc, wchar_t* argv[], wchar_t* envp[])
    {
        do_run(argc, argv);
    }

    void do_run(int argc, wchar_t* argv[])
    {
        auto pr = boost::program_options::wcommand_line_parser(argc, argv)
                .options(m_options_description)
                /*
                 * program_options库使用的程序选项默认是UNIX风格，使用双连字符和单连字符指定选项名，但如果用户喜欢，也可以改为使用Windows风格的斜杠
                */
                .style(boost::program_options::command_line_style::unix_style | boost::program_options::command_line_style::allow_slash_for_short)
                /*
                 * 允许未定义选项
                */
                .allow_unregistered()
                .run();

        /*
         * 当使用了allow_unregistered()函数后，command_line_parser遇到未定义但又“像”是选项的命令行参数时，
         * 会把他保存在分析结果parsed_options对象中，而不是抛出异常。这些无法识别的参数可以使用collect_unrecognized()获得
        */
        std::vector<std::wstring> ur_opts = boost::program_options::collect_unrecognized(pr.options, boost::program_options::include_positional);

        /* 保存解析结果到variables_map_ */
        /* store()函数允许多次调用，实现同时从不同的数据源获得选项信息，比如同时使用命令行和配置文件，已经有值的选项如果再进行store()会自动忽略，所以
         * 对于容器类型的选项，需要指定composing用来合并防止自动忽略
        */
        boost::program_options::store(pr, m_variables_map);
        /* 把值更新到用户指定的变量 */
        boost::program_options::notify(m_variables_map);
    }

private:
    boost::program_options::options_description m_options_description;
    /* variables_map的键类型是string，他存放了选项描述器option_description定义的选项名。
     * 在使用variables_map时我们只能使用选项的长名，使用短名索引会发生运行时异常
     *
     * variables_map的值类型是any对象的一个包装类variable_value，他的成员函数as<T>()包
     * 装了any_cast<T>()，取出any内存放的选项值，因此as<T>()函数的模板类型必须与存储的类型相符，否则会抛出bad_any_cast异常
    */
    boost::program_options::variables_map m_variables_map;
};

KIWI_FAST_CLOSE_UTILITY_NAMESPACE

#define SERVICE_COMMAND_LINE_ADAPTER_METHOD(imp_class)       \

