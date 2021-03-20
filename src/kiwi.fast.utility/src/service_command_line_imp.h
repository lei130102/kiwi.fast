#pragma once

#include <kiwi.fast.utility/detail/config.h>

#include <kiwi.fast.plugin_utility/code_conversion.h>
#include <kiwi.fast.plugin_utility/exceptions.h>

#include <boost/program_options.hpp>

#include <optional>
#include <any>
#include <vector>
#include <type_traits>

KIWI_FAST_OPEN_UTILITY_NAMESPACE

/*!
 * \brief The command_line class
 * 借助boost::program_options管理命令行参数，但并不用其配置文件功能
 */
class service_command_line_imp
{
public:
    virtual void parse(int argc, wchar_t* argv[], wchar_t* envp[])
    {
        do_parse(argc, argv, envp);
    }

    virtual void parse(int argc, wchar_t* argv[])
    {
        do_parse(argc, argv);
    }

    virtual void output_options_description()
    {
        std::cout << m_options_description << '\n';
    }

    virtual std::size_t variable_count(const char8_t* name)
    {
        return m_variables_map.count(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(name));
    }

    virtual bool variable_empty()
    {
        return m_variables_map.empty();
    }

    virtual std::optional<std::any> variable_as_any(const char8_t* name)
    {
        if (variable_count(name))
        {
            //注意at函数返回的引用类型，往外传递时都要保持使用的是这个引用类型
            //注意这里一定要返回boost::program_options::variable_value类型的引用是因为值存储在这里面并且可以开出接口让之后运行的程序代码可以修改其中的值
            return std::ref(m_variables_map.at(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(name)));
        }
        else
        {
            return {};
        }
    }

    virtual void add_option(const char8_t* name, const char8_t* description)
    {
        m_options_description.add_options()
                (KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(name).c_str()                  //注意，要求类型是const char*
                    , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(description).c_str());    //注意，要求类型是const char*
    }

    template<typename ValueType>
    void add_value_option(const char8_t* name
        , const char8_t* description
        , std::optional<ValueType> default_value_ = {}     //没有指定选项时的默认值
        , std::optional<ValueType> implicit_value_ = {})   //指定选项但没有指定值时的默认值
    {
        typename boost::program_options::typed_value<ValueType, std::conditional_t<std::is_same_v<ValueType, bool>, char, wchar_t>>* typed_value = nullptr;
        if constexpr (std::is_same_v<ValueType, bool>)
        {
            typed_value = boost::program_options::bool_switch(); //类似boost::program_options::value<bool>()，不同的是他的值没有用，只要指定选型就是true
            //注意这个没有提供wchar_t的版本，所以在命令行中尽量不要提供值
            //没有std::vector<bool>
            //通过variable<bool>()获取的不是跟在选项后面的值
        }
        else
        {
            typed_value = boost::program_options::wvalue<ValueType>();
        }

        //注意，不管是value还是wvalue，单参数版本default_value函数内部都是调用boost::lexical_cast<std::string>(v);(注意是写死的std::string)，所以对宽字符wchar_t、char8_t都是不支持的
        //同理implicit_value也一样
        if constexpr (std::is_same_v<ValueType, std::wstring>)//这里设计不再为除了std::wstring外的没有对ostream的operator<<操作的类型提供此便利，因为命令行长度有限制，更复杂的功能由配置文件的方式替代
        {
            if (default_value_)
            {
                typed_value->default_value(*default_value_, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(*default_value_));    //不要求，由第二个参数替代该操作
            }

            if (implicit_value_)
            {
                typed_value->implicit_value(*implicit_value_, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(*implicit_value_));
            }
        }
        else
        {
            if (default_value_)
            {
                typed_value->default_value(*default_value_);    //要求decltype(*default_value_)类型必须有对ostream的operator<<操作  (因为内部借助boost::lexcial_cast转换从而导致的要求)
            }

            if (implicit_value_)
            {
                typed_value->implicit_value(*implicit_value_);
            }
        }

		m_options_description.add_options()
			(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(name).c_str()
                , typed_value
				, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(description).c_str());
    }

    //boost::program_options不支持deque，但支持vector
    template<typename ValueType>
    void add_vector_value_option(const char8_t* name
        , const char8_t* description
        , std::optional<std::vector<ValueType>> default_value_ = {}     //没有指定选项时的默认值
        , std::optional<std::vector<ValueType>> implicit_value_ = {})   //指定选项但没有指定值时的默认值
    {
        typename boost::program_options::typed_value<std::vector<ValueType>, wchar_t>* typed_value = nullptr;
        typed_value = boost::program_options::wvalue<std::vector<ValueType>>();

        if (default_value_)
        {
            typed_value->default_value(*default_value_, "");
        }

        if (implicit_value_)
        {
            typed_value->implicit_value(*implicit_value_, "");
        }
        //    typed_value* multitoken();              //设置多记号属性
        //    typed_value* zero_tokens();             //设置零记号属性
        //(目前测试表明，zero_tokens，multitoken只适用于T是vector的情况)

        //program_options.exe --optname1 --optname4 ""             (因为optname1指定了zero_tokens，所以可以不指定选项值，zero_tokens针对那些存在意义大于值意义的选项，否则抛出异常)
        //输出：optname1:

        //program_options.exe --optname1 1.1 --optname4 ""
        //输出：optname1:1.1

        //program_options.exe --optname1 1.1 1.2 --optname4 ""     (因为optname1指定了multitoken，否则optname1:1.1)


        typed_value->multitoken()->zero_tokens();

        m_options_description.add_options()
        	(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(name).c_str()
                , typed_value
        		, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(description).c_str());
    }





    //没有bool   std::vector<bool>    只有为bool提供的变种bool_switch
    virtual void add_bool_switch_option(const char8_t* name
        , const char8_t* description
        , std::optional<bool> default_value_ = {})
    {
        add_value_option<bool>(name, description, default_value_);
    }

    //没有wchar_t    std::vector<wchar_t>   和   char8_t    std:vector<char8_t>
    //没有char       std::vector<char>     boost::lexical_cast does not support narrowing of character types.Use boost::locale instead 

    //std::wstring       
    //std::vector<std::wstring>
    virtual void add_wstring_option(const char8_t* name
        , const char8_t* description
        , std::optional<std::wstring> default_value_ = {}     
        , std::optional<std::wstring> implicit_value_ = {})   
    {
        add_value_option<std::wstring>(name, description, default_value_, implicit_value_);
    }
    virtual void add_vector_wstring_option(const char8_t* name
        , const char8_t* description
        , std::optional<std::vector<std::wstring>> default_value_ = {}     
        , std::optional<std::vector<std::wstring>> implicit_value_ = {})   
    {
        add_vector_value_option<std::wstring>(name, description, default_value_, implicit_value_);
    }

    //short
    //std::vector<short>
    virtual void add_short_option(const char8_t* name
        , const char8_t* description
        , std::optional<short> default_value_ = {}     
        , std::optional<short> implicit_value_ = {})   
    {
        add_value_option<short>(name, description, default_value_, implicit_value_);
    }
    virtual void add_vector_short_option(const char8_t* name
        , const char8_t* description
        , std::optional<std::vector<short>> default_value_ = {}     
        , std::optional<std::vector<short>> implicit_value_ = {})   
    {
        add_vector_value_option<short>(name, description, default_value_, implicit_value_);
    }

    //unsigned short  
    //std::vector<unsigned short>
    virtual void add_unsigned_short_option(const char8_t* name
        , const char8_t* description
        , std::optional<unsigned short> default_value_ = {}     
        , std::optional<unsigned short> implicit_value_ = {})   
    {
        add_value_option<unsigned short>(name, description, default_value_, implicit_value_);
    }
    virtual void add_vector_unsigned_short_option(const char8_t* name
        , const char8_t* description
        , std::optional<std::vector<unsigned short>> default_value_ = {}     
        , std::optional<std::vector<unsigned short>> implicit_value_ = {})   
    {
        add_vector_value_option<unsigned short>(name, description, default_value_, implicit_value_);
    }

    //int     
    //std::vector<int>
    virtual void add_int_option(const char8_t* name
        , const char8_t* description
        , std::optional<int> default_value_ = {}     
        , std::optional<int> implicit_value_ = {})   
    {
        add_value_option<int>(name, description, default_value_, implicit_value_);
    }
    virtual void add_vector_int_option(const char8_t* name
        , const char8_t* description
        , std::optional<std::vector<int>> default_value_ = {}     
        , std::optional<std::vector<int>> implicit_value_ = {})   
    {
        add_vector_value_option<int>(name, description, default_value_, implicit_value_);
    }

    //unsigned int        
    //std::vector<unsigned int>
    virtual void add_unsigned_int_option(const char8_t* name
        , const char8_t* description
        , std::optional<unsigned int> default_value_ = {}     
        , std::optional<unsigned int> implicit_value_ = {})   
    {
        add_value_option<unsigned int>(name, description, default_value_, implicit_value_);
    }
    virtual void add_vector_unsigned_int_option(const char8_t* name
        , const char8_t* description
        , std::optional<std::vector<unsigned int>> default_value_ = {}     
        , std::optional<std::vector<unsigned int>> implicit_value_ = {})   
    {
        add_vector_value_option<unsigned int>(name, description, default_value_, implicit_value_);
    }

    //long long           
    //std::vector<long long>
    virtual void add_long_long_option(const char8_t* name
        , const char8_t* description
        , std::optional<long long> default_value_ = {}     
        , std::optional<long long> implicit_value_ = {})   
    {
        add_value_option<long long>(name, description, default_value_, implicit_value_);
    }
    virtual void add_vector_long_long_option(const char8_t* name
        , const char8_t* description
        , std::optional<std::vector<long long>> default_value_ = {}     
        , std::optional<std::vector<long long>> implicit_value_ = {})   
    {
        add_vector_value_option<long long>(name, description, default_value_, implicit_value_);
    }

    //unsigned long long     
    //std::vector<unsigned long long>
    virtual void add_unsigned_long_long_option(const char8_t* name
        , const char8_t* description
        , std::optional<unsigned long long> default_value_ = {}     
        , std::optional<unsigned long long> implicit_value_ = {})   
    {
        add_value_option<unsigned long long>(name, description, default_value_, implicit_value_);
    }
    virtual void add_vector_unsigned_long_long_option(const char8_t* name
        , const char8_t* description
        , std::optional<std::vector<unsigned long long>> default_value_ = {}     
        , std::optional<std::vector<unsigned long long>> implicit_value_ = {})   
    {
        add_vector_value_option<unsigned long long>(name, description, default_value_, implicit_value_);
    }

    //float         
    //std::vector<float>
    virtual void add_float_option(const char8_t* name
        , const char8_t* description
        , std::optional<float> default_value_ = {}     
        , std::optional<float> implicit_value_ = {})   
    {
        add_value_option<float>(name, description, default_value_, implicit_value_);
    }
    virtual void add_vector_float_option(const char8_t* name
        , const char8_t* description
        , std::optional<std::vector<float>> default_value_ = {}     
        , std::optional<std::vector<float>> implicit_value_ = {})   
    {
        add_vector_value_option<float>(name, description, default_value_, implicit_value_);
    }

    //double        
    //std::vector<double>
    virtual void add_double_option(const char8_t* name
        , const char8_t* description
        , std::optional<double> default_value_ = {}     
        , std::optional<double> implicit_value_ = {})   
    {
        add_value_option<double>(name, description, default_value_, implicit_value_);
    }
    virtual void add_vector_double_option(const char8_t* name
        , const char8_t* description
        , std::optional<std::vector<double>> default_value_ = {}     
        , std::optional<std::vector<double>> implicit_value_ = {})   
    {
        add_vector_value_option<double>(name, description, default_value_, implicit_value_);
    }

    virtual std::optional<std::any> variable_as_any_kiwi_fast_cpu_mask()
    {
        return variable_as_any(u8"kiwi_fast_cpu_mask");
    }

protected:
    service_command_line_imp()
        :m_options_description(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"所有选项"), 500)  //注意这里写死是500，所以描述信息不要长
    {}

    virtual ~service_command_line_imp()
    {}

    /*!
     * 添加缺省选项(注意缺省选项名不要跟用户自定义添加的重复)
     */
    void add_default_option()
    {
        //cpu掩码，用来决定当前进程在哪个核心上运行
        m_options_description.add_options()
            (KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"kiwi_fast_cpu_mask").c_str()    //注意名称要唯一
                , boost::program_options::wvalue<unsigned int>()                                                        
                , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"进程CPU掩码").c_str());
    }

    /*!
     * 添加帮助选项
     * test.exe --help
     * test.exe -h
     * 注意这里设计使用帮助选项后，程序退出
     */
    void add_help_option()
    {
        m_options_description.add_options()
                (KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"help,h").c_str()
                    , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"帮助信息").c_str());
    }

    void do_parse(int argc, wchar_t* argv[], wchar_t* envp[])
    {
        do_parse(argc, argv);
    }

    void do_parse(int argc, wchar_t* argv[])
    {
        add_default_option();
        add_help_option();

        try
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
        catch (...)
        {
            KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, u8"解析命令行参数失败。可能：命令行中没有指定没有implicit_value的选项的选项值");
        }

        if (variable_count(u8"help"))
        {
            output_options_description();
            exit(0);  //退出程序
        }
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

#define SERVICE_COMMAND_LINE_ADAPTER_METHOD(imp_class)                                                        \
    void parse(int argc, wchar_t* argv[], wchar_t* envp[])                                                    \
    {                                                                                                         \
        imp_class::parse(argc, argv, envp);                                                                   \
    }                                                                                                         \
    void parse(int argc, wchar_t* argv[])                                                                     \
    {                                                                                                         \
        imp_class::parse(argc, argv);                                                                         \
    }                                                                                                         \
    void output_options_description()                                                                         \
    {                                                                                                         \
        imp_class::output_options_description();                                                              \
    }                                                                                                         \
    std::size_t variable_count(const char8_t* name)                                                           \
    {                                                                                                         \
        return imp_class::variable_count(name);                                                               \
    }                                                                                                         \
    bool variable_empty()                                                                                     \
    {                                                                                                         \
        return imp_class::variable_empty();                                                                   \
    }                                                                                                         \
    std::optional<std::any> variable_as_any(const char8_t* name)                                              \
    {                                                                                                         \
        return imp_class::variable_as_any(name);                                                              \
    }                                                                                                         \
    void add_option(const char8_t* name, const char8_t* description)                                          \
    {                                                                                                         \
        imp_class::add_option(name, description);                                                             \
    }                                                                                                         \
    void add_bool_switch_option(const char8_t* name                                                           \
        , const char8_t* description                                                                          \
        , std::optional<bool> default_value = {})                                                             \
    {                                                                                                         \
        imp_class::add_bool_switch_option(name, description, default_value);                                  \
    }                                                                                                         \
    void add_wstring_option(const char8_t* name                                                               \
        , const char8_t* description                                                                          \
        , std::optional<std::wstring> default_value = {}                                                      \
        , std::optional<std::wstring> implicit_value = {})                                                    \
    {                                                                                                         \
        imp_class::add_wstring_option(name, description, default_value, implicit_value);                      \
    }                                                                                                         \
    void add_vector_wstring_option(const char8_t* name                                                        \
        , const char8_t* description                                                                          \
        , std::optional<std::vector<std::wstring>> default_value = {}                                         \
        , std::optional<std::vector<std::wstring>> implicit_value = {})                                       \
    {                                                                                                         \
        imp_class::add_vector_wstring_option(name, description, default_value, implicit_value);               \
    }                                                                                                         \
    void add_short_option(const char8_t* name                                                                 \
        , const char8_t* description                                                                          \
        , std::optional<short> default_value = {}                                                             \
        , std::optional<short> implicit_value = {})                                                           \
    {                                                                                                         \
        imp_class::add_short_option(name, description, default_value, implicit_value);                        \
    }                                                                                                         \
    void add_vector_short_option(const char8_t* name                                                          \
        , const char8_t* description                                                                          \
        , std::optional<std::vector<short>> default_value = {}                                                \
        , std::optional<std::vector<short>> implicit_value = {})                                              \
    {                                                                                                         \
        imp_class::add_vector_short_option(name, description, default_value, implicit_value);                 \
    }                                                                                                         \
    void add_unsigned_short_option(const char8_t* name                                                        \
        , const char8_t* description                                                                          \
        , std::optional<unsigned short> default_value = {}                                                    \
        , std::optional<unsigned short> implicit_value = {})                                                  \
    {                                                                                                         \
        imp_class::add_unsigned_short_option(name, description, default_value, implicit_value);               \
    }                                                                                                         \
    void add_vector_unsigned_short_option(const char8_t* name                                                 \
        , const char8_t* description                                                                          \
        , std::optional<std::vector<unsigned short>> default_value = {}                                       \
        , std::optional<std::vector<unsigned short>> implicit_value = {})                                     \
    {                                                                                                         \
        imp_class::add_vector_unsigned_short_option(name, description, default_value, implicit_value);        \
    }                                                                                                         \
    void add_int_option(const char8_t* name                                                                   \
        , const char8_t* description                                                                          \
        , std::optional<int> default_value = {}                                                               \
        , std::optional<int> implicit_value = {})                                                             \
    {                                                                                                         \
        imp_class::add_int_option(name, description, default_value, implicit_value);                          \
    }                                                                                                         \
    void add_vector_int_option(const char8_t* name                                                            \
        , const char8_t* description                                                                          \
        , std::optional<std::vector<int>> default_value = {}                                                  \
        , std::optional<std::vector<int>> implicit_value = {})                                                \
    {                                                                                                         \
        imp_class::add_vector_int_option(name, description, default_value, implicit_value);                   \
    }                                                                                                         \
    void add_unsigned_int_option(const char8_t* name                                                          \
        , const char8_t* description                                                                          \
        , std::optional<unsigned int> default_value = {}                                                      \
        , std::optional<unsigned int> implicit_value = {})                                                    \
    {                                                                                                         \
        imp_class::add_unsigned_int_option(name, description, default_value, implicit_value);                 \
    }                                                                                                         \
    void add_vector_unsigned_int_option(const char8_t* name                                                   \
        , const char8_t* description                                                                          \
        , std::optional<std::vector<unsigned int>> default_value = {}                                         \
        , std::optional<std::vector<unsigned int>> implicit_value = {})                                       \
    {                                                                                                         \
        imp_class::add_vector_unsigned_int_option(name, description, default_value, implicit_value);          \
    }                                                                                                         \
    void add_long_long_option(const char8_t* name                                                             \
        , const char8_t* description                                                                          \
        , std::optional<long long> default_value = {}                                                         \
        , std::optional<long long> implicit_value = {})                                                       \
    {                                                                                                         \
        imp_class::add_long_long_option(name, description, default_value, implicit_value);                    \
    }                                                                                                         \
    void add_vector_long_long_option(const char8_t* name                                                      \
        , const char8_t* description                                                                          \
        , std::optional<std::vector<long long>> default_value = {}                                            \
        , std::optional<std::vector<long long>> implicit_value = {})                                          \
    {                                                                                                         \
        imp_class::add_vector_long_long_option(name, description, default_value, implicit_value);             \
    }                                                                                                         \
    void add_unsigned_long_long_option(const char8_t* name                                                    \
        , const char8_t* description                                                                          \
        , std::optional<unsigned long long> default_value = {}                                                \
        , std::optional<unsigned long long> implicit_value = {})                                              \
    {                                                                                                         \
        imp_class::add_unsigned_long_long_option(name, description, default_value, implicit_value);           \
    }                                                                                                         \
    void add_vector_unsigned_long_long_option(const char8_t* name                                             \
        , const char8_t* description                                                                          \
        , std::optional<std::vector<unsigned long long>> default_value = {}                                   \
        , std::optional<std::vector<unsigned long long>> implicit_value = {})                                 \
    {                                                                                                         \
        imp_class::add_vector_unsigned_long_long_option(name, description, default_value, implicit_value);    \
    }                                                                                                         \
    void add_float_option(const char8_t* name                                                                 \
        , const char8_t* description                                                                          \
        , std::optional<float> default_value = {}                                                             \
        , std::optional<float> implicit_value = {})                                                           \
    {                                                                                                         \
        imp_class::add_float_option(name, description, default_value, implicit_value);                        \
    }                                                                                                         \
    void add_vector_float_option(const char8_t* name                                                          \
        , const char8_t* description                                                                          \
        , std::optional<std::vector<float>> default_value = {}                                                \
        , std::optional<std::vector<float>> implicit_value = {})                                              \
    {                                                                                                         \
        imp_class::add_vector_float_option(name, description, default_value, implicit_value);                 \
    }                                                                                                         \
    void add_double_option(const char8_t* name                                                                \
        , const char8_t* description                                                                          \
        , std::optional<double> default_value = {}                                                            \
        , std::optional<double> implicit_value = {})                                                          \
    {                                                                                                         \
        imp_class::add_double_option(name, description, default_value, implicit_value);                       \
    }                                                                                                         \
    void add_vector_double_option(const char8_t* name                                                         \
        , const char8_t* description                                                                          \
        , std::optional<std::vector<double>> default_value = {}                                               \
        , std::optional<std::vector<double>> implicit_value = {})                                             \
    {                                                                                                         \
        imp_class::add_vector_double_option(name, description, default_value, implicit_value);                \
    }                                                                                                         \
    std::optional<std::any> variable_as_any_kiwi_fast_cpu_mask()                                              \
    {                                                                                                         \
        return imp_class::variable_as_any_kiwi_fast_cpu_mask();                                               \
    }
