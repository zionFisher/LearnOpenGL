# LearnOpenGL 学习笔记

- [LearnOpenGL 学习笔记](#learnopengl-学习笔记)
  - [引言](#引言)
  - [Advanced OpenGL | 高级 OpenGL](#advanced-opengl--高级-opengl)
    - [Advanced Data | 高级数据](#advanced-data--高级数据)
      - [`glBufferData` 和 `glBufferSubData`](#glbufferdata-和-glbuffersubdata)
      - [分批顶点属性](#分批顶点属性)
      - [`glCopyBufferSubData` 复制缓冲](#glcopybuffersubdata-复制缓冲)
    - [Advanced GLSL | 高级 GLSL](#advanced-glsl--高级-glsl)
      - [GLSL 内置变量(built-in variables)](#glsl-内置变量built-in-variables)
      - [接口块(glsl 结构体)](#接口块glsl-结构体)
      - [`Uniform` 缓冲对象与 `Uniform` 块](#uniform-缓冲对象与-uniform-块)
      - [`Uniform` 块布局](#uniform-块布局)
      - [使用 `Uniform` 缓冲和 `Uniform` 块](#使用-uniform-缓冲和-uniform-块)
    - [GPU Instancing | GPU 实例化](#gpu-instancing--gpu-实例化)
  - [附录](#附录)
    - [Buffer Binding Target | 缓冲目标](#buffer-binding-target--缓冲目标)
    - [GLSL built-in variables | GLSL 内置变量](#glsl-built-in-variables--glsl-内置变量)

## 引言
**********
&emsp;&emsp;为了更好地总结在 LearnOpenGL 上学到的知识，我会将已学部分的**总结内容**放到这篇 markdown 里。

## Advanced OpenGL | 高级 OpenGL
**********
&emsp;&emsp;这一部分是高级 OpenGL，主要包含了 [Advanced Data | 高级数据](#advanced-data--高级数据)，[Advanced GLSL | 高级 GLSL](#advanced-glsl--高级-glsl)，[GPU Instancing | GPU 实例化](#gpu-instancing--gpu-实例化)

### Advanced Data | 高级数据
**********
&emsp;&emsp;首先我们要明确一点，OpenGL 中的**缓冲对象**(简称缓冲)仅仅是管理内存块儿的对象。我们只有在将它绑定到一个**缓冲目标**(Buffer Target)时，才赋予了其意义。具体来说，对于一个 `VBO`，我们只有将其绑定到 `GL_ARRAY_BUFFER` 时，才赋予其顶点数组缓冲的意义。<br/>
&emsp;&emsp;此外，缓冲对象可以有很多个，但是每一种缓冲目标只有一个。

#### `glBufferData` 和 `glBufferSubData`
&emsp;&emsp;此前我们一直使用 `glBufferData` 填充缓存对象所管理的内存，`glBufferData` 分配整块内存，并用给定数据初始化这块内存(可以不初始化内存)。而 `glBufferSubData` 则可以填充缓冲的特定区域，这个函数声明中包含了一个偏移量，使得我们能够在指定内存的指定区域填充数据。我们对比一下两个函数的声明:
```cpp
void glBufferData(GLenum target,
 	              GLsizeiptr size,
 	              const void * data,
 	              GLenum usage);

void glBufferSubData(GLenum target,
 	                 GLintptr offset,
 	                 GLsizeiptr size,
 	                 const void * data);
```
&emsp;&emsp;此外，对一个缓冲调用 `glBufferSubData` 之前必须先调用 `glBufferData`。

#### 分批顶点属性
&emsp;&emsp;此前教程中顶点数组缓冲的布局是交错式的，也就是:
```
position (3 float) | normal (3 float) | texCoord (2 float)
position (3 float) | normal (3 float) | texCoord (2 float)
...
position (3 float) | normal (3 float) | texCoord (2 float)
```
&emsp;&emsp;而使用 glBufferSubData 能够让缓冲的布局呈以下形式:
```
position (3 float)
position (3 float)
position (3 float)
...
normal (3 float)
normal (3 float)
normal (3 float)
...
texCoord (2 float)
texCoord (2 float)
texCoord (2 float)
```
&emsp;&emsp;第二种无疑是更直观，更利于导入的，具体实现:
```cpp
float positions[] = { ... };
float normals[] = { ... };
float tex[] = { ... };

glBufferData(GL_ARRAY_BUFFER,
             sizeof(position) + sizeof(normals) + sizeof(tex),
             NULL, // set to NULL
             GL_STATIC_DRAW)

glBufferSubData(GL_ARRAY_BUFFER, 0,
                sizeof(positions), &positions);
glBufferSubData(GL_ARRAY_BUFFER, sizeof(positions),
                sizeof(normals), &normals);
glBufferSubData(GL_ARRAY_BUFFER, sizeof(positions) + sizeof(normals),
                sizeof(tex), &tex);

glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                      0);
glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                      (void*)(sizeof(positions)));
glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
                      (void*)(sizeof(positions) + sizeof(normals)));
```

#### `glCopyBufferSubData` 复制缓冲
&emsp;&emsp;如果我们想将一个缓冲中的内容复制到另一个缓冲中，我们可以使用 `glCopyBufferSubData` 函数(注意，没有 `glCopyBufferData` 函数)，它的函数原型:
```cpp
void glCopyBufferSubData(GLenum readTarget,
                         GLenum writeTarget,
                         GLintptr readOffset,
                         GLintptr writeOffset,
                         GLsizeiptr size);
```
&emsp;&emsp;其中 `readTarget` 和 `writeTarget` 都是缓冲目标，使用示例:
```cpp
float vertexData[] = { ... };
glBindBuffer(GL_COPY_READ_BUFFER, vbo1);
glBindBuffer(GL_COPY_WRITE_BUFFER, vbo2);
glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, sizeof(vertexData));
// 下面这种形式也可以
float vertexData[] = { ... };
glBindBuffer(GL_ARRAY_BUFFER, vbo1);
glBindBuffer(GL_COPY_WRITE_BUFFER, vbo2);
glCopyBufferSubData(GL_ARRAY_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, sizeof(vertexData));
```

### Advanced GLSL | 高级 GLSL
**********
&emsp;&emsp;了解一些高级 GLSL 特性有助于我们写出更高效优质的代码。
#### GLSL 内置变量(built-in variables)
&emsp;&emsp;下面的表格提供了常见的 GLSL 内置变量，具体用法请移步 OpenGL Reference，更多相关变量请移步 [GLSL 内置变量](#glsl-built-in-variables--glsl-内置变量)。
| Name             | Type    | Description                                                                                     |
| ---------------- | ------- | ----------------------------------------------------------------------------------------------- |
| `gl_Position`    | `vec4`  | 齐次裁剪空间坐标                                                                                |
| `gl_PointSize`   | `float` | 使用 `GL_POINTS` 图元时，可以用于控制渲染出来的点的大小                                         |
| `gl_VertexID`    | `int`   | 只读，保存当前顶点的整数索引                                                                    |
| `gl_FragCoord`   | `vec4`  | 只读，保存当前片元的坐标，x和y分量是窗口坐标，z分量是深度值                                     |
| `gl_FrontFacing` | `bool`  | 只读，如果当前片元是 `Front Face` 则为 `true`，否则为 `false`                                   |
| `gl_FragDepth`   | `float` | 可以通过这个内置变量，修改片元的深度值。使用这个变量可能导致 Early-Z 失效，但存在一些弥补手段。 |

#### 接口块(glsl 结构体)
&emsp;&emsp;使用接口块，我们能更方便地组织变量(`in`, `out`, `uniform`)，接口块地定义类似结构体，一个具体的例子是:
```glsl
// 使用接口块作为 in out 变量时，需要在顶点着色器和片元着色器中都进行声明，
// 且必须同名。
// 下例中，VS_OUT 作为块名是一致的，但是实例 vs_out 和 fs_in 可以不一致。
// in vertex shader
out VS_OUT
{
    vec2 TexCoords;
} vs_out;

// in fragment shader
in VS_OUT
{
    vec2 TexCoords;
} fs_in;
```
&emsp;&emsp;相较于将接口块称作接口块，个人认为将其看作结构体即可。

#### `Uniform` 缓冲对象与 `Uniform` 块
&emsp;&emsp;`uniform` 变量让我们可以将变量从 CPU 发送到 GPU，但我们也遇到了一些麻烦。比如说，当使用多于一个着色器时，尽管不同着色器的 `uniform` 变量值可能是相同的，但我们仍然需要为每个着色器重复设置这些相同的 `uniform` 变量。具体来说，`view` 和 `projection` 矩阵可能对于每个着色器都是一样的，但我们需要为每个着色器都设置一遍。<br/>
&emsp;&emsp;导致这种现象的主要原因是，单个着色器的 `uniform` 变量是**局部**的，是仅存在于当前着色器内部的。为了解决这个问题，我们可以使用 OpenGL 提供的叫做 `uniform` 缓冲对象的工具，来创建一系列在多个着色器中相同的**全局** `uniform` 变量。<br/>
&emsp;&emsp;正如它的名字，`uniform` 缓冲对象也是缓冲的一种，我们可以使用 `glGenBuffers` 来创建它。而在 shader 中，我们需要用 `uniform` 块来获取 `uniform` 缓冲对象的数据。如下示例:
```glsl
#version 330 core
layout (location = 0) in vec3 aPos;

// uniform 块
layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

uniform mat4 model;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
```
&emsp;&emsp;和普通的接口块不同，`uniform` 块无需定义实例。`layout (std140)` 则代表了一种 `uniform` 块布局。

#### `Uniform` 块布局
&emsp;&emsp;所谓 `uniform` 块布局是一种类似 C++ 结构体内存对齐的概念。我们最常使用的是 `layout (std140)` 布局。该布局的具体规范可以在[这里](https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_uniform_buffer_object.txt)找到，此处只列出最常见规则。GLSL 中的每个变量，比如说 `bool`、`int` 和 `float`，都被定义为 4 字节。每 4 字节用一个 `N` 来表示。每个变量都有一个对齐量，它等于该变量在 `uniform` 块中所占的实际空间，而偏移量则可以理解为变量相较于结构体头部的偏移大小。如果你熟悉 C++ 中的内存对齐，相信这部分内容比较好理解，为了和教程保持一致，我仍然采用了对齐量和偏移量的说法。
| 类型                 | 对齐量                                           |
| -------------------- | ------------------------------------------------ |
| 标量(`int`,`bool`等) | 每个标量按 `N` 对齐                              |
| 向量                 | 按 `2N` 或 `4N` 对齐                             |
| 数组                 | 每个元素都按 `4N` 对齐                           |
| 矩阵                 | 等价于列向量数组，每个向量按 `4N` 对齐           |
| 结构体(接口块)       | 所有元素按上述规则计算完毕后，补齐到 `4N` 的倍数 |

&emsp;&emsp;此外，每个变量的偏移量必须是自身对齐量的倍数。我们来看一个具体例子:
```glsl
layout (std140) uniform ExampleBlock
{
                     // 对齐量           // 偏移量
    float value;     // N               // 0
    vec3 vector;     // 4N              // 16  (必须是4N的倍数，所以是16)
    mat4 matrix;     // 4N              // 32  (列 0)
                     // 4N              // 48  (列 1)
                     // 4N              // 64  (列 2)
                     // 4N              // 80  (列 3)
    float values[3]; // 4N              // 96  (values[0])
                     // 4N              // 112 (values[1])
                     // 4N              // 128 (values[2])
    bool boolean;    // N               // 144
    int integer;     // N               // 148
};
```

#### 使用 `Uniform` 缓冲和 `Uniform` 块
&emsp;&emsp;(为了更好的理解这部分内容，请务必区分好 `uniform` 缓冲和 `uniform` 块的具体定义)<br/>
&emsp;&emsp;首先我们需要创建一个缓冲，然后绑定到 `GL_UNIFORM_BUFFER`，并调用 `glBufferData` 分配内存:
```glsl
unsigned int uboExampleBlock;
glGenBuffers(1, &uboExampleBlock);
glBindBuffer(GL_UNIFORM_BUFFER, uboExampleBlock);
// 分配152字节的内存
glBufferData(GL_UNIFORM_BUFFER, 152, NULL, GL_STATIC_DRAW);
glBindBuffer(GL_UNIFORM_BUFFER, 0);
```
&emsp;&emsp;我们可以通过 `glBufferSubData` 来更新 `uniform` 块中的变量数据。<br/>
&emsp;&emsp;现在我们已经创建了 `uniform` 缓冲了，也知道如何更新其数据，但如何让 OpenGL 知道 shader 中的 `uniform` 块对应的是哪个 `uniform` 缓冲呢？<br/>
&emsp;&emsp;OpenGL提供了一种**绑定点**机制(Binding Point)，我们可以将一个缓冲链接到某个点上，然后将需要使用该缓冲数据的块也绑定到这个点上。这样，`uniform` 块就成功使用了指定 `uniform` 缓冲的数据了。示意图如下:
![绑定点](./resources/markdown/advanced_glsl_binding_points.png)
&emsp;&emsp;具体操作上，我们需要使用 `glUniformBlockBinding` 函数绑定 `uniform` 块到绑定点上:
```glsl
// glGetUniformBlockIndex 函数获取 shaderA 的 uniform 块 Lights 的位置索引
// 这个索引是 glUniformBlockBinding 的参数
unsigned int lights_index = glGetUniformBlockIndex(shaderA.ID, "Lights");
// 此处将 shaderA 的 uniform 块 Lights 绑定到绑定点2
glUniformBlockBinding(shaderA.ID, lights_index, 2);
```
&emsp;&emsp;OpenGL 4.2 后，我们可以不用这么麻烦，只需要在声明 `uniform` 块时添加一个布局标识符即可:
```glsl
// 与使用 glGetUniformBlockIndex 和 glUniformBlockBinding 等价
layout(std140, binding = 2) uniform Lights { ... };
```
&emsp;&emsp;此外，我们还需要将 `uniform` 缓冲对象绑定到绑定点上:
```glsl
glBindBufferBase(GL_UNIFORM_BUFFER, 2, uboExampleBlock);
// 或
glBindBufferRange(GL_UNIFORM_BUFFER, 2, uboExampleBlock, 0, 152);
```
&emsp;&emsp;上面两种方法等价，但是第二种能够只将缓冲对象中的一部分绑定到绑定点上，灵活性更强。而更新缓冲中的某个变量只需要使用 `glBufferSubData` 就能实现:
```glsl
glBindBuffer(GL_UNIFORM_BUFFER, uboExampleBlock);
int b = true; // GLSL中的bool是4字节的，所以我们将它存为一个integer
glBufferSubData(GL_UNIFORM_BUFFER, 144, 4, &b);
glBindBuffer(GL_UNIFORM_BUFFER, 0);
```

### GPU Instancing | GPU 实例化
**********



## 附录
**********
&emsp;&emsp;这部分主要包含 OpenGL 常用内容。

### Buffer Binding Target | 缓冲目标
**********
&emsp;&emsp;常见缓冲目标有以下几种:
| Buffer Binding Target     | Purpose                    |
| ------------------------- | -------------------------- |
| `GL_ARRAY_BUFFER`         | 顶点数组缓冲               |
| `GL_COPY_READ_BUFFER`     | 缓冲复制时充当 readTarget  |
| `GL_COPY_WRITE_BUFFER`    | 缓冲复制时充当 writeTarget |
| `GL_ELEMENT_ARRAY_BUFFER` | 顶点索引数组缓冲           |
| `GL_TEXTURE_BUFFER`       | 纹理数据缓冲               |

### GLSL built-in variables | GLSL 内置变量
**********
| Name             | Type    | Description                                                                                     |
| ---------------- | ------- | ----------------------------------------------------------------------------------------------- |
| `gl_Position`    | `vec4`  | 齐次裁剪空间坐标                                                                                |
| `gl_PointSize`   | `float` | 使用 `GL_POINTS` 图元时，可以用于控制渲染出来的点的大小                                         |
| `gl_VertexID`    | `int`   | 只读，保存当前顶点的整数索引                                                                    |
| `gl_FragCoord`   | `vec4`  | 只读，保存当前片元的坐标，x和y分量是窗口坐标，z分量是深度值                                     |
| `gl_FrontFacing` | `bool`  | 只读，如果当前片元是 `Front Face` 则为 `true`，否则为 `false`                                   |
| `gl_FragDepth`   | `float` | 可以通过这个内置变量，修改片元的深度值。使用这个变量可能导致 Early-Z 失效，但存在一些弥补手段。 |