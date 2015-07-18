# SphinxAPI、SphinxQL和SphinxSE的区别
SPhinxAPI，SPhinxAPI默认提供,php,python,java，这三种言语的API。当然你也可以开发其他语言的API。
SPhinxAPI使用要用到 一些set，比如设置搜索服务，匹配模式，排序依据，结果返回，分页，获取摘要，高亮等。具体请查看http://www.coreseek.cn/docs/coreseek_4.1-sphinx_2.0.1-beta.html#api-reference
SPhinxAPI是通过在索引文件里查到符合条件的ID，然后再根据ID在数据源里查到具体的信息，再显示。

SphinxQL查询，是通过mysql协议查询，可以做到实时，效率要比SPhinxAPI SphinxSE要高。
在Sphinx.conf里配置  index rt ,需要指定协议端口，和索引字段，字段要在SphinxQL里用到。关于具体的QL语法请浏览http://www.coreseek.cn/docs/coreseek_4.1-sphinx_2.0.1-beta.html#sphinxql-referencehttp://www.coreseek.cn/docs/coreseek_4.1-sphinx_2.0.1-beta.html#sphinxql-reference
与标准的mysql 语句有一定区别，此外SphinxQL还提供了API之外的功能。附，查询语法

SELECT
    select_expr [, select_expr ...]
    FROM index [, index2 ...]
    [WHERE where_condition]
    [GROUP BY {col_name | expr_alias}]
    [ORDER BY {col_name | expr_alias} {ASC | DESC} [, ...]]
    [WITHIN GROUP ORDER BY {col_name | expr_alias} {ASC | DESC}]
    [LIMIT offset, row_count]
    [OPTION opt_name = opt_value [, ...]]

SphinxSE 是一个mysql引擎插件，目前官方提供的是插件补丁是5.0的，也支持5.1,coreseek做了一个5.5的。
5.5版安装教程 http://www.coreseek.cn/products-install/mysql5-5-sphinxse/ 注意区别Sphinx版本，coreseek自己提供的不需要补丁文件，Sphinx官方版本需要使用补丁文件。
别，mysql已经安装上的，和未安装的，安装这个插件的方式也不同。
5.1 5.0版本 http://www.coreseek.cn/docs/coreseek_4.1-sphinx_2.0.1-beta.html#sphinxse-installing
SphinxSE只提供查询功能，和API一样。
使用SphinxSE 需要先建立一个搜索表，引擎当然是新安装的SphinxSE。创建的表的字段也是有要求的，具体的请查看http://www.coreseek.cn/docs/coreseek_4.1-sphinx_2.0.1-beta.html#sphinxse-using
ShpinxSE的优点就是查询方便。

# access searchd via regular MySQL API (python)
import _mysql
db=_mysql.connect(read_default_file="~/.mysql.sphinx.cnf")
db.get_host_info()    'should print: localhost via TCP/IP'
db.get_server_info()  'should print the server version info.'
db.query("SELECT * FROM test1 WHERE MATCH('test')")
r=db.store_result()
r.fetch_row()

~/.mysql.sphinx.cnf:
[client]
port = 9306
protocol = TCP

note: when using mysql command line do
mysql --port 9306 --protocol=TCP

# refs:Sphinx API类型以及SphinxSE实现原理分析
http://blog.csdn.net/jucrazy/article/details/6739162
