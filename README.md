# ODPro

Aplicação desktop em **C++17** desenvolvida como Trabalho de Conclusão de Curso (TCC), voltada ao dimensionamento e à análise de vigas de concreto protendido.

O programa reúne entrada de dados, esquemas da viga e da seção transversal, visualização do traçado dos cabos, gráficos e apresentação dos resultados em uma interface gráfica.

## Funcionalidades

- Entrada de geometria, materiais, carregamentos e parâmetros de protensão.
- Dimensionamento e verificações implementadas de estados limites de serviço e do ato da protensão.
- Cálculo de perdas de protensão e visualização gráfica dos resultados.
- Exportação da viga atual em CSV, para abertura em planilhas, e em TXT.
- Geração e avaliação de vigas aleatórias para experimentação.

Este é um software acadêmico. Os resultados dependem das hipóteses e verificações implementadas; a compilação do programa não constitui validação dos cálculos nem certificação de conformidade normativa.

## Requisitos

- Windows: o Makefile utiliza `cmd.exe` e comandos específicos desse sistema.
- GCC/MinGW com suporte a C++17 e GNU Make. Visual Studio e MSVC não são o ambiente configurado no Makefile.
- raylib compilada para a mesma arquitetura e toolchain do programa.
- Suporte gráfico a OpenGL 3.3.

O Makefile espera, por padrão:

```text
C:/raylib/
├── raylib/src/
│   ├── raylib.h
│   ├── libraylib.a
│   ├── raylib.rc.data
│   └── external/
└── w64devkit/bin/
    ├── g++.exe
    └── mingw32-make.exe
```

Obtenha a raylib e as instruções de instalação no [projeto oficial](https://github.com/raysan5/raylib). Apenas baixar os cabeçalhos não basta: o Makefile também precisa de `libraylib.a` e `raylib.rc.data`.

## Bibliotecas externas

As bibliotecas podem ser instaladas localmente, sem serem incluídas no repositório do ODPro. Baixe os pacotes completos das fontes oficiais, preservando seus arquivos de licença.

| Biblioteca | Versão identificada no ambiente original | Destino esperado |
|---|---|---|
| [raylib](https://github.com/raysan5/raylib) | 6.0, declarada no cabeçalho instalado | `C:/raylib/raylib`, fora do projeto |
| [Dear ImGui](https://github.com/ocornut/imgui/tree/v1.90.6) | 1.90.6 | `external/imgui/` |
| [ImPlot](https://github.com/epezent/implot) | 1.0, declarada no cabeçalho local | `external/implot/` |
| [rlImGui](https://github.com/raylib-extras/rlImGui) | Revisão não identificada na cópia local | `external/rlimgui/` |

Extraia o conteúdo de cada pacote diretamente no destino indicado. Por exemplo, deve existir `external/imgui/imgui.h`, e não `external/imgui/imgui-1.90.6/imgui.h`.

```text
ODPro/
├── src/
│   └── main.cpp
├── external/
│   ├── imgui/
│   │   ├── imgui.h
│   │   ├── imgui.cpp
│   │   ├── imgui_draw.cpp
│   │   ├── imgui_tables.cpp
│   │   ├── imgui_widgets.cpp
│   │   └── ... demais cabeçalhos e licença
│   ├── implot/
│   │   ├── implot.h
│   │   ├── implot_internal.h
│   │   ├── implot.cpp
│   │   ├── implot_items.cpp
│   │   └── ... demais arquivos e licença
│   └── rlimgui/
│       ├── rlImGui.h
│       ├── rlImGui.cpp
│       ├── imgui_impl_raylib.h
│       ├── extras/
│       └── ... demais arquivos e licenças
├── resources/
│   └── web/custom_shell.html
├── Makefile
└── run.bat
```

**Compatibilidade:** os números acima foram lidos nos arquivos locais; não identificam necessariamente o commit exato nem eventuais alterações. A revisão do rlImGui precisa ser registrada para tornar a instalação totalmente reproduzível. A versão mais recente de cada biblioteca não foi validada com este código. Preserve a combinação original e, ao substituir dependências, recompile para verificar compatibilidade. A cópia local de rlImGui também contém `rlgl.h` versão 4.5; esse arquivo pode influenciar a seleção de cabeçalhos.

As pastas `external/eigen/` e `external/cereal/` estavam no ambiente de desenvolvimento, mas não são utilizadas pelo código atual. O RapidJSON está dentro de cereal. **Não é necessário instalar Eigen, cereal ou RapidJSON para esta versão.** O comentário inicial do Makefile ainda menciona Eigen/RapidJSON e há um caminho de inclusão para Eigen; isso não implica uso dessas bibliotecas.

## Compilar e executar

Abra o **PowerShell na raiz do ODPro**. Com o ambiente nos caminhos padrão:

```powershell
& 'C:\raylib\w64devkit\bin\mingw32-make.exe' PROJECT_NAME=ODPro
.\run.bat
```

O executável será gerado em `build/ODPro.exe`. O parâmetro `PROJECT_NAME=ODPro` fixa esse nome mesmo que a pasta tenha sido baixada do GitHub como `ODPro-main`.

Compilação conferida em 11/09/2026, em uma cópia do projeto com as dependências locais originais, raylib com cabeçalho 6.0 e GCC 15.2.0, sem Eigen/cereal/RapidJSON. O executável foi gerado com sucesso; houve avisos de parâmetro não utilizado e de uso de `std::wstring_convert` obsoleto. Não foram validados a execução gráfica, os cálculos nem uma instalação nova das dependências baixadas da internet.

Para compilar e executar em um único comando:

```powershell
& 'C:\raylib\w64devkit\bin\mingw32-make.exe' run PROJECT_NAME=ODPro
```

Para usar outros caminhos, substitua os exemplos abaixo pelos diretórios da sua instalação; prefira caminhos sem espaços:

```powershell
& 'D:\dev\w64devkit\bin\mingw32-make.exe' PROJECT_NAME=ODPro RAYLIB_PATH=D:/dev/raylib COMPILER_PATH=D:/dev/w64devkit/bin
```

Para uma recompilação limpa em modo de depuração, execute os comandos separadamente:

```powershell
& 'C:\raylib\w64devkit\bin\mingw32-make.exe' clean
& 'C:\raylib\w64devkit\bin\mingw32-make.exe' PROJECT_NAME=ODPro BUILD_MODE=DEBUG
```

O alvo `clean` remove `obj/` e `build/`. Ao trocar bibliotecas ou modo de compilação, faça uma recompilação limpa. O Makefile também remove o executável anterior antes de tentar gerar outro; se a compilação falhar, corrija o erro antes de usar `run.bat`.

## Uso

1. Abra o programa e preencha a aba **Entrada e geometria**.
2. Acione o dimensionamento e consulte as páginas de esquemas, gráficos e resultados.
3. Na aba **Resumo**, utilize os botões de exportação.

As exportações são gravadas no diretório de execução como `resultado_viga_interface.csv` e `resultado_viga_interface.txt`. Com `run.bat`, esse diretório é a raiz do projeto. Exportações seguintes podem substituir os arquivos de mesmo nome.

O programa tenta carregar fontes já instaladas no sistema, como Segoe UI, Arial e Tahoma. Não é necessário copiar essas fontes para o projeto. O rlImGui também inclui dados de ícones Font Awesome Free, sujeitos às licenças próprias dessa dependência.

## Outros sistemas e versão web

O Makefile possui alvos `web` e `serve`, mas eles não constituem uma versão web validada. Exigem Emscripten, uma raylib compilada para web em `RAYLIB_PATH/src/web/libraylib.a` e adaptação/verificação do ciclo de execução e da exportação de arquivos. O procedimento documentado acima é para Windows desktop; Linux e macOS exigem adaptação da compilação.

## Créditos e licenças

A interface utiliza raylib, Dear ImGui, ImPlot e rlImGui. Font Awesome Free é incluído por rlImGui. Cada componente conserva sua própria licença e autoria; os autores dessas bibliotecas não são apresentados como autores dos cálculos do ODPro.

O licenciamento do código autoral do ODPro ainda precisa ser definido pelo titular. A publicação deste README não concede uma licença ao código. As licenças das dependências não são substituídas pela futura licença do ODPro.
