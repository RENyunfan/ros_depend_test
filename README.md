# ROS-Depend-Test

```
Yunfan REN
renyunfan@outlook.com
```

在使用ros的过程中，我们常常需要讲不同的功能包编译为动态链接库进行协作开发，然而在使用`catkin_make`的时候，ROS会按照默认的顺序对功能包进行编译。这导致了有的被依赖的包尚未被编译，未生成`xxxConfig.cmake`文件，因此也无法被其他包的`findpackage()`命令找到。因此需要利用`package.xml`文件来提示ROS首先编译依赖包。以前从来没动过这个文件，今天就来详细的分析一下。

本文使用的测试代码托管在：https://github.com/RENyunfan/ros_depend_test

# 0 简短版

如果你不想看那么多，那么简短的说：

## 被依赖库的包

```cmake
# 这里的两行非常重要，它意味着将头文件和库文件进行export
catkin_package(
	INCLUDE_DIRS include
    LIBRARIES my_math_lib
)

include_directories(
	#SYSTEM
	include 
    ${catkin_INCLUDE_DIRS}

)
# 添加库文件
add_library(my_math_lib
        src/my_math_lib.cpp
        )
target_link_libraries( my_math_lib
        ${catkin_LIBRARIES}
        )
```

## 使用依赖库的包

```xml

  <!-- 声明需要依赖的库 -->  
  <build_depend>my_math_lib</build_depend>
  <exec_depend>my_math_lib</exec_depend>
```

```cpp
find_package(catkin REQUIRED COMPONENTS
        roscpp
        # 在配置好了xml文件后，就可以通过findpackage找到自己写的包了
		my_math_lib
        )

catkin_package(
	CATKIN_DEPENDS  my_math_lib
)
# 没有这个可能会找不到头文件
include_directories(
	${catkin_INCLUDE_DIRS}
)

add_executable(main
        src/main.cpp
        )
target_link_libraries( main
        ${catkin_LIBRARIES}
        )
```



# 1 package.xml

## 1.1 必要标签

首先xml是一种文本标记语言，根标签为

```xml
<?xml version="1.0"?>
<package format="2">
	...
</package>
```

内容上来说，ROS要求有一些必要的标签，分别是

* \<name>：包名。
* \<version>:版本号。
* \<description>:对包的简单描述。
* \<maintainer>:负责维护包的人以及联系方式。
* \<license>:包所遵从的协议证书。

一个简单的例子如下

```xml
<package format="2">
  <name>foo_core</name>
  <version>1.2.4</version>
  <description>
  This package provides foo capability.
  </description>
  <maintainer email="renyunfan@outlook.com">Yunfan</maintainer>
  <license>BSD</license>
</package>
```

以上内容说明了一个包的基本信息，接下来我们要添加这个包的依赖信息描述。

## 1.2 依赖标签

一个ROS包中最多可以有六种依赖，分别是

* Build Dependencies
  * 指出当前包编译是所依赖的包。这可以包括在编译时来自这些程序包的标头，与这些程序包中的库链接或在构建时需要任何其他资源（尤其是在CMake中使用find_package（）编辑这些程序包时）。 在交叉编译方案中，构建依赖关系是针对目标体系结构的。
* Build Export Dependencies
  * （构建导出依赖关系）指定了针对该软件包构建库所需的软件包。 在您将其标头可传递地包含在此程序包的公共标头中时，就是这种情况（尤其是当这些程序包在CMake中的catkin_package（）中声明为（CATKIN_）DEPENDS时）。
* Execution Dependencies
  * 执行依赖项指定运行此包中的代码需要哪些包。当您依赖于这个包中的共享库时，就会出现这种情况(特别是当这些包在 CMake 中被声明为(CATKIN _) debs 中的 CATKIN _ package ()时)。
* Test Dependencies
  * 测试依赖项仅指定单元测试的附加依赖项。它们不应该复制任何已经提到的构建或运行依赖项。
* Build Tool Dependencies
  * 生成工具依赖项指定生成系统工具，这个包需要自己生成这些工具。通常，唯一需要的构建工具是 catkin。在跨编译场景中，构建工具的依赖关系是针对执行编译的体系结构的。
* Documentation Tool Dependencies 
  * 文档工具依赖项指定这个包生成文档所需的文档工具。

这六种依赖关系分别由如下的标签定义

```xml
<package format="2">
  <name>foo_core</name>
  <version>1.2.4</version>
  <description>
  This package provides foo capability.
  </description>
  <maintainer email="renyunfan@outlook.com">Yunfan</maintainer>
  <license>BSD</license>
  <author>Yunfan</author>
    
  <depend>roscpp</depend>
  <build_depend>message_generation</build_depend>
  <exec_depend>rospy</exec_depend>
  <test_depend>python-mock</test_depend>
  <buildtool_depend>catkin</buildtool_depend>
  <doc_depend>doxygen</doc_depend>
    
</package>
```

这里我们只需要重点关注build_depend和exec_depend，它指明了这个包需要预先编译哪个包

# 2 CMakeLists

在配置玩完`package.xml`之后，我们需要配置对应的`CMakeList.txt`来完成链接库的生成。

首先要导出包的路径

## 2.1 catkin_package()

catkin_package()是一个catkin提供的CMake宏，用于将catkin特定的信息信息输出到构建系统上，用于生成pkg配置文件以及CMake文件。

这个命令必须在add_library()或者add_executable()之前调用，该函数有5个可选参数：

- INCLUDE_DIRS - 导出包的include路径
- LIBRARIES - 导出项目中的库
- CATKIN_DEPENDS - 该项目依赖的其他catkin项目
- DEPENDS - 该项目所依赖的非catkin CMake项目。
- CFG_EXTRAS - 其他配置选项

## 2.2 add_library

```cmake
add_library(my_math_lib
        src/my_math_lib.cpp
        )
target_link_libraries( my_math_lib
        ${catkin_LIBRARIES}
        )
```

# 3 实例

最后我们给出一个交叉编译库的实例，实现一个跨ROS包的编译。

```cpp
.
├── CMakeLists.txt -> /opt/ros/melodic/share/catkin/cmake/toplevel.cmake
├── complex_test
│   ├── CMakeLists.txt
│   ├── package.xml
│   └── src
│       └── main.cpp
└── my_math_lib
    ├── CMakeLists.txt
    ├── include
    │   └── my_math_lib
    ├── package.xml
    └── src
        └── my_math_lib.cpp
```

如上所示，我们创建了两个包，一个包`my_math_lib`实现了负数的加法运算和`cout`重载，另一个包要使用这个包中的类。

## 3.1 my_math_lib

### package_xml

```xml
<?xml version="1.0"?>
<package format="2">
  <name>my_math_lib</name>
  <version>1.2.4</version>
  <description>
  This package is a test package for cross compile.
  </description>
  <maintainer email="renyunfan@outlook.com">Yunfan</maintainer>
  <license>GLPv3</license>
    
  <!-- 使用catkin进行编译 -->
  <buildtool_depend>catkin</buildtool_depend>
  <!-- 使用roscpp库 -->  
  <build_depend>roscpp</build_depend>
  <build_export_depend>roscpp</build_export_depend>
  <exec_depend>roscpp</exec_depend>


</package>
```

### CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 2.8.3)
project(my_math_lib)

find_package(catkin REQUIRED COMPONENTS
        roscpp
        )

# 这里的两行非常重要，它意味着将头文件和库文件进行export
catkin_package(
	INCLUDE_DIRS include
    LIBRARIES my_math_lib
)

include_directories(
	#SYSTEM
	include 
    ${catkin_INCLUDE_DIRS}

)
# 添加库文件
add_library(my_math_lib
        src/my_math_lib.cpp
        )
target_link_libraries( my_math_lib
        ${catkin_LIBRARIES}
        )

```

## 3.2 complex_test

### package.xml

```xml
<?xml version="1.0"?>
<package format="2">
  <name>complex_test</name>
  <version>1.2.4</version>
  <description>
  This package is a test package for cross compile.
  </description>
  <maintainer email="renyunfan@outlook.com">Yunfan</maintainer>
  <license>GLPv3</license>

  <buildtool_depend>catkin</buildtool_depend>
  <build_depend>roscpp</build_depend>
  <build_export_depend>roscpp</build_export_depend>
  <exec_depend>roscpp</exec_depend>

  <!-- 声明需要依赖的库 -->  
  <build_depend>my_math_lib</build_depend>
  <exec_depend>my_math_lib</exec_depend>


</package>

```

### CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 2.8.3)
project(complex_test)

find_package(catkin REQUIRED COMPONENTS
        roscpp
        # 在配置好了xml文件后，就可以通过findpackage找到自己写的包了
		my_math_lib
        )


catkin_package(
	CATKIN_DEPENDS  my_math_lib
)
# 没有这个可能会找不到头文件
include_directories(
	${catkin_INCLUDE_DIRS}
)

add_executable(main
        src/main.cpp
        )
target_link_libraries( main
        ${catkin_LIBRARIES}
        )

```

### 主函数

```cpp
#include "iostream"
#include "my_math_lib/my_math_lib.h"
using namespace std;
int main(){

	my_math_lib::Complex<int> a(12,30);
	my_math_lib::Complex<int> b(2,10);
	my_math_lib::Complex<int> c = a + b;
	
	cout<<c<<endl;

	return 0;
}
```

### 运行结果

```bash
(base) ➜  test_ws rosrun complex_test main
14 + 40i
```

