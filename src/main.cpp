#include <raylib.h>
#include "rlImGui.h"
#include "imgui.h"
#include "implot.h"

#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <array>

#include <random>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <codecvt>
#include <locale>

ImVec2 tamanho_paginas_geral = ImVec2(1500.0f, 950.0f);

double InterpolarValores(double x1, double y1,double x,double x2,double y2){
    return (x1 - x) * (y1 - y2) / (x1 - x2 ) + y1;
};

double InterpolarTabela(std::vector<std::array<double, 2>>& tabela, double x) {
    int quantidade_pontos = tabela.size();

    if (quantidade_pontos == 0) {
        return 0.0;
    }

    if (x <= tabela[0][0]) {
        return tabela[0][1];
    }

    for (int i = 0; i < quantidade_pontos - 1; i++) {
        double x1 = tabela[i][0];
        double y1 = tabela[i][1];

        double x2 = tabela[i + 1][0];
        double y2 = tabela[i + 1][1];

        if (x >= x1 && x <= x2) {
            double y = y1 + (x - x1) * (y2 - y1) / (x2 - x1);
            return y;
        }
    }

    return tabela[quantidade_pontos - 1][1];
}

double menorValor(double a, double b)
    {
        if (a < b)
        {
            return a;
        }

        return b;
    }

double maiorValor(double a, double b)
    {
        if (a > b)
        {
            return a;
        }

        return b;
    }

double GerarAleatorioEntre(double minimo, double maximo) {
    static std::random_device rd;
    static std::mt19937 gerador(rd());

    std::uniform_real_distribution<double> distribuicao(minimo, maximo);

    return distribuicao(gerador);
}

double GerarAleatorioArredondado(double min, double max, double multiplo){
    double n = GerarAleatorioEntre (min, max);

    n = round (n / multiplo) * multiplo;

    return n;
}

int GerarInteiroAleatorio(int minimo, int maximo) {
    static std::random_device rd;
    static std::mt19937 gerador(rd());

    std::uniform_int_distribution<int> distribuicao(minimo, maximo);

    return distribuicao(gerador);
}

enum ClasseAgressividadeAmbiental {
    CAA_I,
    CAA_II,
    CAA_III,
    CAA_IV
};

enum TipoCombinacaoServico {
    Combinacao_Frequente,
    Combinacao_Quase_Permanente,
    Combinacao_Rara
};

enum NivelProtensao {
    Protensao_Nivel_1_Parcial,
    Protensao_Nivel_2_Limitada,
    Protensao_Nivel_3_Completa
};

enum TipoAcaoVariavel {
    Acao_Residencias_Hospedagens_Acesso_Restrito,
    Acao_Comercio_Escritorios_Lojas_Publicos,
    Acao_Bibliotecas_Arquivos_Oficinas_Garagens,
    Acao_Vento,
    Acao_Temperatura
};

enum EfeitoAcao {
    Efeito_Desfavoravel,
    Efeito_Favoravel
};

std::string NomeCombinacaoServico(TipoCombinacaoServico combinacao)
{
    if (combinacao == Combinacao_Rara) {
        return "rara";
    }
    else if (combinacao == Combinacao_Frequente) {
        return "frequente";
    }

    return "quase permanente";
}

std::string NomeNivelProtensao(NivelProtensao nivel)
{
    if (nivel == Protensao_Nivel_1_Parcial) {
        return "nível 1 - protensão parcial";
    }
    else if (nivel == Protensao_Nivel_2_Limitada) {
        return "nível 2 - protensão limitada";
    }

    return "nível 3 - protensão completa";
}

double Psi1PorTipoAcaoVariavel(TipoAcaoVariavel tipo_acao_variavel)
{
    if (tipo_acao_variavel == Acao_Comercio_Escritorios_Lojas_Publicos) {
        return 0.6;
    }
    else if (tipo_acao_variavel == Acao_Bibliotecas_Arquivos_Oficinas_Garagens) {
        return 0.7;
    }
    else if (tipo_acao_variavel == Acao_Vento) {
        return 0.3;
    }
    else if (tipo_acao_variavel == Acao_Temperatura) {
        return 0.5;
    }

    return 0.4;
}

double Psi2PorTipoAcaoVariavel(TipoAcaoVariavel tipo_acao_variavel)
{
    if (tipo_acao_variavel == Acao_Comercio_Escritorios_Lojas_Publicos) {
        return 0.4;
    }
    else if (tipo_acao_variavel == Acao_Bibliotecas_Arquivos_Oficinas_Garagens) {
        return 0.6;
    }
    else if (tipo_acao_variavel == Acao_Vento) {
        return 0.0;
    }
    else if (tipo_acao_variavel == Acao_Temperatura) {
        return 0.3;
    }

    return 0.3;
}

double FatorCombinacaoServico(TipoCombinacaoServico combinacao, TipoAcaoVariavel tipo_acao_variavel)
{
    if (combinacao == Combinacao_Rara) {
        return 1.0;
    }
    else if (combinacao == Combinacao_Frequente) {
        return Psi1PorTipoAcaoVariavel(tipo_acao_variavel);
    }

    return Psi2PorTipoAcaoVariavel(tipo_acao_variavel);
}

double GammaAcaoPermanente(EfeitoAcao efeito)
{
    if (efeito == Efeito_Favoravel) {
        return 1.0;
    }

    return 1.4;
}

double GammaAcaoVariavel(EfeitoAcao efeito)
{
    if (efeito == Efeito_Favoravel) {
        return 0.0;
    }

    return 1.4;
}

double GammaProtensao(EfeitoAcao efeito)
{
    if (efeito == Efeito_Favoravel) {
        return 0.9;
    }

    return 1.2;
}

TipoCombinacaoServico CombinacaoELSFPorNivelProtensao(NivelProtensao nivel_protensao)
{
    if (nivel_protensao == Protensao_Nivel_3_Completa) {
        return Combinacao_Rara;
    }

    return Combinacao_Frequente;
}

TipoCombinacaoServico CombinacaoELSDPorNivelProtensao(NivelProtensao nivel_protensao)
{
    if (nivel_protensao == Protensao_Nivel_2_Limitada) {
        return Combinacao_Quase_Permanente;
    }
    else if (nivel_protensao == Protensao_Nivel_3_Completa) {
        return Combinacao_Frequente;
    }

    return Combinacao_Frequente;
}

enum TipoEstrutura {
    Concreto_Simples,
    Concreto_Armado,
    Concreto_Protendido
};

enum ElementoEstrutural {
    Laje,
    Pilar,
    Elemento_em_contato_com_solo,
    Viga
};

enum TipoCimento {
    CP_I,
    CP_II,
    CP_III,
    CP_IV,
    CP_V_ARI
};

enum TipoAgregado {
    AGREGADO_BASALTO,
    AGREGADO_GRANITO,
    AGREGADO_CALCARIO,
    AGREGADO_ARENITO
};

enum TipoAcoPassivo {
    CA_50,
    CA_60
};

enum TipoAcoAtivo{
    CP_190_RB,
    CP_190_RN,
    CP_210_RB,
    CP_210_RN
};

enum TipoProtensao{
    Pre_tracionada,
    Pos_tracionada_aderente,
    Pos_tracionada_nao_aderente,
    Barras_de_aco
};

enum TipoCaboeProtensao{
    Cabo_e_Concreto,
    Barras_ou_fios_com_mossas_ou_saliencias_e_bainha_metalica,
    Fios_lisos_ou_cordoalhas_e_bainha_metalica,
    Fios_lisos_ou_cordoalhas_e_bainha_metalica_lubrificada,
    Cordoalha_e_bainha_polipropileno_lubrificada
};

double CoeficienteAtritoCurvaturaPorTipoCabo(TipoCaboeProtensao tipo_cabo_e_protensao)
{
    if (tipo_cabo_e_protensao == Cabo_e_Concreto) {
        return 0.50;
    }
    else if (tipo_cabo_e_protensao == Barras_ou_fios_com_mossas_ou_saliencias_e_bainha_metalica) {
        return 0.30;
    }
    else if (tipo_cabo_e_protensao == Fios_lisos_ou_cordoalhas_e_bainha_metalica) {
        return 0.20;
    }
    else if (tipo_cabo_e_protensao == Fios_lisos_ou_cordoalhas_e_bainha_metalica_lubrificada) {
        return 0.10;
    }

    return 0.05;
}


struct CondicoesClima{
    double temperatura_media = 0.0;
    double umidade_media = 0.0;

    CondicoesClima(double temperatura_media_, double umidade_media_){
        temperatura_media = temperatura_media_;
        umidade_media = umidade_media_;
    }
};

struct Concreto{
    double fck_MPa; // resistência característica do concreto
    double fck;
    double gama_c; // coeficiente de ponderação do concreto
    double peso_especifico; // peso específico do concreto
    double alfa_E; // parâmetro para módulo de elasticidade de acordo com agregado
    double alfa_i; //de modulo elasticidade inicial para secante

    double fcd;
    double lambda;
    double eta_c;
    double alfa_c;
    double modulo_elasticidade_inicial;
    double modulo_elasticidade_secante;
    double coeficiente_relaxacao = 0.0;

    TipoCimento tipo_cimento;
    TipoAgregado tipo_agregado;
    

    int abatimento_min_cm = 5;
    int abatimento_max_cm = 9;
    double alfa_fluencia = 0.0;
    double alfa_retracao = 1.0;
    double s = 0.0;

    double fctm_MPa;

    double fck_j_MPa; 
    double fctm_j_MPa;
    int n_dias;

    Concreto(double fck_MPa_, TipoCimento tipo_cimento_, TipoAgregado tipo_agregado_)
    :tipo_cimento(tipo_cimento_),
    tipo_agregado(tipo_agregado_)
    {
        fck_MPa = fck_MPa_;
        fck = fck_MPa * 1.0e6;
        gama_c = 1.4;
        peso_especifico = 25000.0;

        calcularPropriedades();
    }

    void calcularPropriedades(){
        fcd = fck / gama_c;
        
        if(tipo_agregado == AGREGADO_ARENITO){
            alfa_E = 0.7;
        } else if(tipo_agregado == AGREGADO_BASALTO){
            alfa_E = 1.2;
        } else if(tipo_agregado == AGREGADO_CALCARIO){
            alfa_E = 0.9;
        }else{
            alfa_E = 1.0;
        }

        if (fck_MPa <= 50.0){
            fctm_MPa = 0.3 * std:: pow(fck_MPa, (2.0/3.0));
            modulo_elasticidade_inicial = alfa_E * 5600.0 * std::sqrt(fck_MPa);
            lambda = 0.8;
            alfa_c = 0.85;

            if (fck_MPa <= 40.0){
                eta_c = 1.0;}
            else{
                eta_c = std::pow(40.0 / fck_MPa, 1.0 / 3.0);
            }}
        else{
            fctm_MPa = 2.12 * std:: log( 1.0 + 0.1 * (fck_MPa + 8.0));
            modulo_elasticidade_inicial = 21.5 * 1e3 * alfa_E * std:: pow( (fck_MPa / 10 + 1.25), 1.0/3.0);
            lambda = 0.8 - (fck_MPa - 50.0) / 400.0;
            eta_c = std::pow(40.0 / fck_MPa, 1.0 / 3.0);
            alfa_c = 0.85 * (1.0 - (fck_MPa - 50.0) / 200.0);}

        
        if(tipo_cimento == CP_III ||tipo_cimento == CP_IV) {
            alfa_fluencia = 1.0;
            s = 0.38;
        }else if(tipo_cimento == CP_V_ARI){
            alfa_fluencia = 3.0;
            s = 0.38;
        }else{
            alfa_fluencia = 2.0;
            s = 0.25;
        }

        alfa_i = 0.8 + 0.2 * fck_MPa / 80;

        if(alfa_i > 1){
            alfa_i = 1;
        }

        modulo_elasticidade_secante = alfa_i * modulo_elasticidade_inicial;
        
    }

    double calcularfckj(int n_dias_){
        n_dias = n_dias_;

        fck_j_MPa = fck_MPa * std::exp(s * (1.0 - std::sqrt(28.0 / n_dias)));

        return fck_j_MPa;
    }
    ///avaliarAtoProtensao
    double calcularfctmj(int n_dias_){
        n_dias = n_dias_;
        fck_j_MPa = calcularfckj (n_dias);
        fctm_j_MPa = 0.3 * std::pow(fck_j_MPa, 2.0 / 3.0);
        return fctm_j_MPa;
    }
};

struct AcoPassivo{
    double fyk; // resistência característica do aço passivo
    double gama_s; // coeficiente de ponderação do aço
    double modulo_elasticidade_MPa; // módulo de elasticidade do aço passivo

    double fyd;

    AcoPassivo(double fyk_MPa, double modulo_elasticidade_GPa){
        fyk = fyk_MPa * 1.0e6;
        gama_s = 1.15;
        modulo_elasticidade_MPa = modulo_elasticidade_GPa * 1000.0;

        calcularPropriedades();
    }

    void calcularPropriedades(){
        fyd = fyk / gama_s;
    }
};

struct ArmaduraPassiva{
    AcoPassivo aco;
    ClasseAgressividadeAmbiental classe_agressividade_ambiental;
    TipoEstrutura tipo_estrutura;
    ElementoEstrutural elemento_estrutural;

    double diametro_barra_longitudinal; // diâmetro da armadura longitudinal
    double diametro_estribo; // diâmetro do estribo
    double cobrimento; // cobrimento nominal da armadura
    double cobrimento_mm;

    ArmaduraPassiva(AcoPassivo aco_, double diametro_barra_longitudinal_mm, double diametro_estribo_mm, ClasseAgressividadeAmbiental classe_agressividade_ambiental_, TipoEstrutura tipo_estrutura_, ElementoEstrutural elemento_estrutural_)
     : aco(aco_),
     classe_agressividade_ambiental(classe_agressividade_ambiental_),
     tipo_estrutura(tipo_estrutura_),
     elemento_estrutural(elemento_estrutural_)
     {
        diametro_barra_longitudinal = diametro_barra_longitudinal_mm / 1000.0;
        diametro_estribo = diametro_estribo_mm / 1000.0;

        CalcularCobrimentoNominal();}


    void CalcularCobrimentoNominal () {

        if((tipo_estrutura == Concreto_Protendido && elemento_estrutural == Viga) || (tipo_estrutura == Concreto_Protendido && elemento_estrutural == Pilar)){
        if(classe_agressividade_ambiental == CAA_I){
            cobrimento_mm = 30.0; 

        } else if(classe_agressividade_ambiental == CAA_II){
            cobrimento_mm = 35.0; 

        } else if(classe_agressividade_ambiental == CAA_III){
            cobrimento_mm = 45.0; 

        } else{
            cobrimento_mm = 55.0; 

        }}
        cobrimento = cobrimento_mm / 1000.0;
    };
};

struct AcoAtivo{
    double fptk;
    double fator_fpyk;
    double modulo_elasticidade_MPa;

    TipoAcoAtivo tipo_aco_ativo;
    TipoProtensao tipo_protensao;

    double fpyk;
    double tensao_inicial;

    AcoAtivo(double fptk_MPa, double fator_fpyk_, double modulo_elasticidade_GPa, TipoAcoAtivo tipo_aco_ativo_, TipoProtensao tipo_protensao_)
    :tipo_aco_ativo (tipo_aco_ativo_),
    tipo_protensao (tipo_protensao_)
    {
        fptk = fptk_MPa * 1.0e6;
        fator_fpyk = fator_fpyk_;
        modulo_elasticidade_MPa = modulo_elasticidade_GPa * 1000.0;

        calcularPropriedades();
    }

    void calcularPropriedades(){
        fpyk = fator_fpyk * fptk;

        if(tipo_protensao == Pre_tracionada){
            tensao_inicial = 0.77 * fptk;
        } else if (tipo_protensao == Pos_tracionada_aderente){
            tensao_inicial = 0.74 * fptk;
        } else if (tipo_protensao == Pos_tracionada_nao_aderente){
            tensao_inicial = 0.8 * fptk;
        } else{
                  tensao_inicial = 0.72 * fptk;
        }}
};

struct ArmaduraAtiva{
    AcoAtivo aco;
    TipoCaboeProtensao tipo_cabo_e_protensao;
    NivelProtensao nivel_protensao;
    int numero_cordoalhas;
    double area_cordoalha;

    double area_total;
    double tensao_inicial;
    double forca_inicial;
    double tensao_final;
    double forca_final;
    double coeficiente_atrito_curvatura;

    ArmaduraAtiva(AcoAtivo aco_, int numero_cordoalhas_, double area_cordoalha_cm2, TipoCaboeProtensao tipo_cabo_e_protensao_, NivelProtensao nivel_protensao_) : aco(aco_){
        numero_cordoalhas = numero_cordoalhas_;
        area_cordoalha = area_cordoalha_cm2 * 1.0e-4;
        tipo_cabo_e_protensao = tipo_cabo_e_protensao_;
        nivel_protensao = nivel_protensao_;

        calcularPropriedades();
    }

    void calcularPropriedades(){
        area_total = numero_cordoalhas * area_cordoalha;
        tensao_inicial = aco.tensao_inicial;
        forca_inicial = tensao_inicial * area_total;
        tensao_final = tensao_inicial;
        forca_final = forca_inicial;
        coeficiente_atrito_curvatura = CoeficienteAtritoCurvaturaPorTipoCabo(tipo_cabo_e_protensao);
    }
};

struct SecaoRetangular{
    double altura;
    double largura;

    double area = 0.0;
    double inercia_x = 0.0;
    double inercia_y = 0.0;
    double altura_util = 0.0;
    double y_centroide = 0.0;
    double perimetro_contato_ar = 0.0;
    double espessura_ficticia = 0.0;
    double coeficiente_de_ponderacao= 0.0;

    SecaoRetangular(double altura_cm, double largura_cm){
        altura = altura_cm / 100.0;
        largura = largura_cm / 100.0;
    }

    void calcularGeometria(ArmaduraPassiva armadura_passiva){
        area = largura * altura;
        inercia_x = largura * altura * altura * altura / 12.0;
        inercia_y = altura * largura * largura * largura / 12.0;
        altura_util = altura - armadura_passiva.cobrimento - armadura_passiva.diametro_estribo - armadura_passiva.diametro_barra_longitudinal / 2.0;
        y_centroide = altura / 2.0;
        perimetro_contato_ar = 2 * altura + 2* largura;
    }

    void calcularGeometria(ArmaduraPassiva armadura_passiva, CondicoesClima condicoes_clima){
        area = largura * altura;
        inercia_x = largura * altura * altura * altura / 12.0;
        inercia_y = altura * largura * largura * largura / 12.0;
        altura_util = altura - armadura_passiva.cobrimento - armadura_passiva.diametro_estribo - armadura_passiva.diametro_barra_longitudinal / 2.0;
        y_centroide = altura / 2.0;
        perimetro_contato_ar = 2 * altura + 2* largura;

        if(condicoes_clima.umidade_media <= 90.0){
            coeficiente_de_ponderacao = 1 + std:: exp(-7.8 + 0.1 * condicoes_clima.umidade_media);
        }else{
             std::vector<std::array<double, 2>> tabela_coeficiente_de_ponderacao = {
            {90.0, 5.0},
            {100.0, 30.0}};
            coeficiente_de_ponderacao = InterpolarTabela(tabela_coeficiente_de_ponderacao, condicoes_clima.umidade_media);
        }

        espessura_ficticia = coeficiente_de_ponderacao * 2 * area / perimetro_contato_ar;
    }
};

struct VigaConcretoArmado{
    Concreto concreto;
    ArmaduraPassiva armadura_passiva;
    SecaoRetangular secao;

    double vao; // comprimento da viga
    double MSd; // momento solicitante de cálculo
    double NSd; // força normal solicitante de cálculo
    double qk; // carga distribuída solicitante de cálculo

    double x_linha_neutra = 0.0;
    double x_sobre_d = 0.0;
    double As = 0.0; // área de aço passivo

    VigaConcretoArmado(double vao_, double MSd_kNm, double NSd_kN, double qSd_kN_por_m, Concreto concreto_, ArmaduraPassiva armadura_passiva_, SecaoRetangular secao_) :
        concreto(concreto_), armadura_passiva(armadura_passiva_), secao(secao_){
        vao = vao_;
        MSd = MSd_kNm * 1000.0;
        NSd = NSd_kN * 1000.0;
        qk = qSd_kN_por_m * 1000.0;
    }

    void dimensionar(){
        secao.calcularGeometria(armadura_passiva);
        calcularLinhaNeutra();
        calcularAs();
    }

    void calcularLinhaNeutra(){
        double d = secao.altura_util;
        double termo_raiz = 1.0 - 2.0 * MSd / (concreto.alfa_c * concreto.eta_c * concreto.fcd * secao.largura * d * d);
        x_linha_neutra = d / concreto.lambda * (1.0 - std::sqrt(termo_raiz));
        x_sobre_d = x_linha_neutra / d;
    }

    void calcularAs(){
        double d = secao.altura_util;
        As = MSd / (armadura_passiva.aco.fyd * (d - 0.5 * concreto.lambda * x_linha_neutra));
    }
};

struct VigaConcretoProtendido{
    struct Cabo{
        struct Ponto{
            double x = 0.0;
            double y = 0.0;

            double alfa_acumulado = 0.0;

            double tensao_apos_atrito_MPa = 0.0;
            double tensao_apos_encunhamento_MPa = 0.0;
            double tensao_apos_encurtamento_MPa = 0.0;

            double forca_protensao_kN = 0.0;
            double excentricidade = 0.0;
            double momento_peso_proprio_kNm = 0.0;
            double momento_devido_carga_kNm = 0.0;

            double tensao_concreto_no_nivel_do_cabo_MPa = 0.0;
            double perda_encurtamento_MPa = 0.0;

            double forca_protensao_ato_kN = 0.0;
            double momento_peso_proprio_ato_kNm = 0.0;
            double tensao_topo_ato_MPa = 0.0;
            double tensao_base_ato_MPa = 0.0;
            double limite_compressao_ato_MPa = 0.0;
            double limite_tracao_ato_MPa = 0.0;


            double tensao_no_concreto_pela_protensao_em_t0 = 0.0;
            double eta = 0.0;
            double perda_progressiva_total_MPa = 0.0;
            double percentual_perda_total = 0.0;
            double tensao_apos_perda_progressiva__MPa = 0.0;
            double forca_apos_perda_progressiva_kN = 0.0;


            double tensao_els_f_MPa = 0.0;
            double forca_protensao_els_f_kN = 0.0;
            double limite_tracao_fctf_MPa = 0.0;
            double limite_compressao_els_f_MPa = 0.0;
            double momento_els_f_kNm = 0.0;
            double tensao_topo_els_f_MPa = 0.0;
            double tensao_base_els_f_MPa = 0.0;

            double tensao_els_d_MPa = 0.0;
            double forca_protensao_els_d_kN = 0.0;
            double limite_tracao_els_d_MPa = 0.0;
            double limite_compressao_els_d_MPa = 0.0;
            double momento_els_d_kNm = 0.0;
            double tensao_topo_els_d_MPa = 0.0;
            double tensao_base_els_d_MPa = 0.0;

            double altura_util_cabo_elu = 0.0;
            double excentricidade_elu = 0.0;
            double taxa_geometrica_elu = 0.0;
            double relacao_vao_altura_util_elu = 0.0;
            double limite_delta_sigma_p_elu_MPa = 0.0;
            double delta_sigma_p_elu_MPa = 0.0;
            double tensao_protensao_elu_MPa = 0.0;
            double forca_protensao_elu_kN = 0.0;
            double momento_peso_proprio_elu_kNm = 0.0;
            double momento_carga_elu_kNm = 0.0;
            double momento_protensao_elu_kNm = 0.0;
            double momento_solicitante_elu_kNm = 0.0;
            double momento_resistente_elu_kNm = 0.0;
            double x_linha_neutra_elu = 0.0;
            double x_sobre_d_elu = 0.0;
            double As_elu = 0.0;
            bool elu_aprovado_no_ponto = true;

        };

        struct Trecho{
            double altura_inicial = 0.0;
            double altura_final = 0.0;
            double dx = 0.0;
            double dy = 0.0;

            double beta = 0.0;
            double alfa = 0.0;
            double alfa_acumulado = 0.0;

            double perda_forca_atrito = 0.0;
            double tensao_final_atrito = 0.0;
        };

        ArmaduraAtiva armadura_ativa;

        double altura_do_cabo_no_apoio;
        double excentricidade_max_cabo;
        int numero_pontos;

        double coeficiente_atrito_curvatura;
        double coeficiente_atrito_retilineo;
        double escorregamento_ancoragem;

        bool encunhamento_convergiu = false;
        int iteracoes_encunhamento = 0;
        double area_alvo_encunhamento;
        double area_encunhamento_calculada = 0.0;
        double erro_relativo_encunhamento = 0.0;
        double perda_na_ancoragem_encunhamento_MPa = 0.0;
        bool usar_tracado_pedro;

        std::vector<Ponto> pontos;
        std::vector<Trecho> trechos;

        Cabo(ArmaduraAtiva armadura_ativa_, double altura_do_cabo_no_apoio_cm, double excentricidade_max_cabo_cm, int numero_pontos_, double escorregamento_ancoragem_mm, bool usar_tracado_pedro_ = false) :
            armadura_ativa(armadura_ativa_){
            altura_do_cabo_no_apoio = altura_do_cabo_no_apoio_cm / 100.0;
            excentricidade_max_cabo = excentricidade_max_cabo_cm / 100.0;
            numero_pontos = numero_pontos_;
            escorregamento_ancoragem = escorregamento_ancoragem_mm / 1000.0;
            usar_tracado_pedro = usar_tracado_pedro_;
            armadura_ativa.calcularPropriedades();
            coeficiente_atrito_curvatura = armadura_ativa.coeficiente_atrito_curvatura;
            coeficiente_atrito_retilineo = coeficiente_atrito_curvatura / 100.0;
            area_alvo_encunhamento = escorregamento_ancoragem * armadura_ativa.aco.modulo_elasticidade_MPa;
        }

        void gerarTracado(double vao){
            pontos.clear();
            trechos.clear();

            std::vector<double> y_pedro = {-0.25, -0.2575, -0.28, -0.3081, -0.3325, -0.3531, -0.37, -0.3831, -0.3925, -0.3981,
                -0.40, -0.3981, -0.3925, -0.3831, -0.37 , -0.3531, -0.3325, -0.3081, -0.28, -0.2575, -0.25};

            for (int i = 0; i < maiorValor(numero_pontos, 21); i++){
                Ponto ponto;
                double t = (double)i / (double)(maiorValor(numero_pontos, 21) - 1);
                ponto.x = t * vao;
                ponto.y = altura_do_cabo_no_apoio - excentricidade_max_cabo * 4.0 * t * (1.0 - t);///isso deveria ser considerado centroide? duvida

                if (usar_tracado_pedro){
                    ponto.y = y_pedro[i] + 0.5;
                }   

                pontos.push_back(ponto);
            }
        }

        void avaliarAtrito(){
            trechos.clear();

            double alfa_acumulado = 0.0;
            double beta_anterior = 0.0;

            pontos[0].tensao_apos_atrito_MPa = armadura_ativa.tensao_inicial / 1.0e6;
            pontos[0].alfa_acumulado = 0.0;

            for (int i = 0; i < (int)pontos.size() - 1; i++){
                Trecho trecho;

                trecho.altura_inicial = pontos[i].y;
                trecho.altura_final = pontos[i + 1].y;
                trecho.dx = pontos[i + 1].x - pontos[i].x;
                trecho.dy = pontos[i + 1].y - pontos[i].y;
                trecho.beta = std::atan2(trecho.dy, trecho.dx);

                if (i == 0){
                    trecho.alfa = 0.0;
                }
                else{
                    trecho.alfa = std::abs(trecho.beta - beta_anterior);
                }

                alfa_acumulado = alfa_acumulado + trecho.alfa;
                trecho.alfa_acumulado = alfa_acumulado;

                trecho.perda_forca_atrito =
                    armadura_ativa.forca_inicial
                    * (1.0 - std::exp(-((coeficiente_atrito_curvatura * trecho.alfa_acumulado) + (coeficiente_atrito_retilineo * pontos[i + 1].x))));

                trecho.tensao_final_atrito =
                    armadura_ativa.tensao_inicial
                    - trecho.perda_forca_atrito / armadura_ativa.area_total;

                pontos[i + 1].alfa_acumulado = trecho.alfa_acumulado;
                pontos[i + 1].tensao_apos_atrito_MPa = trecho.tensao_final_atrito / 1.0e6;

                beta_anterior = trecho.beta;
                trechos.push_back(trecho);
            }
        }

        double calcularTensaoTentativaAposEncunhamento(int indice, double perda_na_ancoragem_MPa){
            return pontos[0].tensao_apos_atrito_MPa
                 - perda_na_ancoragem_MPa
                 + (pontos[0].tensao_apos_atrito_MPa - pontos[indice].tensao_apos_atrito_MPa);
        }

        double calcularDiferencaEntreAtritoEEncunhamento(int indice, double perda_na_ancoragem_MPa){
            return pontos[indice].tensao_apos_atrito_MPa - calcularTensaoTentativaAposEncunhamento(indice, perda_na_ancoragem_MPa);
        }

        double calcularAreaAfetadaPeloEncunhamento(double perda_na_ancoragem_MPa){
            double area = 0.0;

            for (int i = 1; i < (int)pontos.size(); i++){
                double x_inicial = pontos[i - 1].x;
                double x_final = pontos[i].x;
                double comprimento_trecho = x_final - x_inicial;

                double diferenca_inicial_MPa = calcularDiferencaEntreAtritoEEncunhamento(i - 1, perda_na_ancoragem_MPa);
                double diferenca_final_MPa = calcularDiferencaEntreAtritoEEncunhamento(i, perda_na_ancoragem_MPa);

                if (diferenca_inicial_MPa > 0.0 && diferenca_final_MPa > 0.0){
                    area = area + 0.5 * (diferenca_inicial_MPa + diferenca_final_MPa) * comprimento_trecho;
                }
                else if (diferenca_inicial_MPa > 0.0 && diferenca_final_MPa <= 0.0){
                    double fator_cruzamento = diferenca_inicial_MPa / (diferenca_inicial_MPa - diferenca_final_MPa);
                    double x_limite_encunhamento = x_inicial + fator_cruzamento * comprimento_trecho;
                    area = area + 0.5 * diferenca_inicial_MPa * (x_limite_encunhamento - x_inicial);
                    break;
                }
                else{
                    break;
                }
            }

            return area;
        }

        double encontrarPerdaNaAncoragemPorArea(){
            encunhamento_convergiu = false;
            iteracoes_encunhamento = 0;
            area_encunhamento_calculada = 0.0;
            erro_relativo_encunhamento = 0.0;
            perda_na_ancoragem_encunhamento_MPa = 0.0;

            double tolerancia_area = 0.001;
            double perda_baixa_MPa = 0.0;
            double perda_alta_MPa = 10.0;
            int tentativas_intervalo = 0;

            while (calcularAreaAfetadaPeloEncunhamento(perda_alta_MPa) < area_alvo_encunhamento && tentativas_intervalo < 100){
                perda_alta_MPa = perda_alta_MPa * 2.0;
                tentativas_intervalo = tentativas_intervalo + 1;
            }

            double perda_meio_MPa = 0.0;

            for (int iteracao = 0; iteracao < 100; iteracao++){
                perda_meio_MPa = 0.5 * (perda_baixa_MPa + perda_alta_MPa);
                area_encunhamento_calculada = calcularAreaAfetadaPeloEncunhamento(perda_meio_MPa);
                iteracoes_encunhamento = iteracao + 1;
                erro_relativo_encunhamento = std::abs(area_encunhamento_calculada - area_alvo_encunhamento) / std::abs(area_alvo_encunhamento);

                if (erro_relativo_encunhamento <= tolerancia_area){
                    encunhamento_convergiu = true;
                    perda_na_ancoragem_encunhamento_MPa = perda_meio_MPa;
                    return perda_meio_MPa;
                }

                if (area_encunhamento_calculada < area_alvo_encunhamento){
                    perda_baixa_MPa = perda_meio_MPa;
                }
                else{
                    perda_alta_MPa = perda_meio_MPa;
                }
            }

            perda_na_ancoragem_encunhamento_MPa = perda_meio_MPa;
            return perda_meio_MPa;
        }

        void avaliarEncunhamento(){
            double perda_na_ancoragem_MPa = encontrarPerdaNaAncoragemPorArea();

            for (int i = 0; i < (int)pontos.size(); i++){
                pontos[i].tensao_apos_encunhamento_MPa = calcularTensaoTentativaAposEncunhamento(i, perda_na_ancoragem_MPa);

                if (pontos[i].tensao_apos_encunhamento_MPa > pontos[i].tensao_apos_atrito_MPa){
                    pontos[i].tensao_apos_encunhamento_MPa = pontos[i].tensao_apos_atrito_MPa;
                }
            }
        }

        double tensaoAposPerdasImediatasNoMeioDoVao(){
            int indice;
            double t;

            if((int)pontos.size() % 2 == 0){
                indice = (int)pontos.size() / 2;
                t = (pontos[indice].tensao_apos_encurtamento_MPa + pontos[indice - 1].tensao_apos_encurtamento_MPa) / 2;

            }else{
                indice = ((int)pontos.size() - 1) / 2;
                t = pontos[indice].tensao_apos_encurtamento_MPa;
            }

            return t;
        }
            
    };

    struct PerdasProgressivas{

    double perda_progressiva_total_MPa = 0.0;

    double percentual_perda_total;
    double tensao_final_MPa = 0.0;

    double forca_inicial_apos_perdas_imediatas_kN = 0.0;
    double forca_final_kN = 0.0;

    double coeficiente_relaxacao_aco = 0.0;

    ///Relaxação
    double coeficiente_fluencia_aco_qui_infinito_percento = 0.0;
    double fator_0_5fptk, fator_0_6fptk, fator_0_7fptk, fator_0_8fptk;
    double fator_tensao_fptk;
    double psi_t_t0_percento;
    double psi_mil_percento;
    double psi_infinito_percento;

    ///Retração
    double retracao_concreto_infinito;
    double retracao_concreto_infinito_t0;
    double idade_ficticia;

    double A, B, C, D, E;
    double beta_s_t ;

    //Fluencia 
    double coeficiente_fluencia = 0.0;
    double phi_f_infinito, beta_f_t, fator_fck_j;
    double phi_a = 0.0;
    double phi_1c = 0.0;
    double phi_f_infinito_t0 = 0.0;
    double phi_d = 0.0;
    double phi_total = 0.0;
    double t = 0.0;

    //total
    double taxa_geometrica_aco;
    double eta;
    double alfa_p;
    double excentricidade_p;
        

    void calcularPerdaPorRelaxacaoAco(ArmaduraAtiva& armadura_ativa, double tensao_no_ponto){//considerando somente cordoalhas

        if(armadura_ativa.aco.tipo_aco_ativo == CP_190_RB || armadura_ativa.aco.tipo_aco_ativo == CP_210_RB){

            std::vector<std::array<double, 2>> tabela_relaxacao_RB = {
            {0.5, 0.0},
            {0.6, 1.3},
            {0.7, 2.5},
            {0.8, 3.5}};

            double fptk_MPa = armadura_ativa.aco.fptk / 1.0e6;
            fator_tensao_fptk = tensao_no_ponto / fptk_MPa;///Delalibera faz com area total,está certo??
            
            if(fator_tensao_fptk <= 0.5){
                psi_t_t0_percento = 0.0;
                psi_mil_percento = 0.0;
            }else {
            psi_mil_percento = InterpolarTabela(tabela_relaxacao_RB, fator_tensao_fptk);}
            

        }
        psi_infinito_percento = 2.5 * psi_mil_percento;

        coeficiente_fluencia_aco_qui_infinito_percento = - std:: log( 1 - psi_infinito_percento / 100) * 100;

    };

    void calcularPerdaPorRetracaoConcreto(Concreto& concreto,  SecaoRetangular& secao, CondicoesClima& condicoes_clima, double dias_ate_ato_protensao){
        double U = condicoes_clima.umidade_media;
        double h = secao.espessura_ficticia;

        idade_ficticia = concreto.alfa_retracao * (condicoes_clima.temperatura_media + 10) / 30 * dias_ate_ato_protensao;

        A = 40.0;
        B = 116 * std:: pow(h, 3) - 282 * std:: pow(h, 2) + 220 * h - 4.8;
        C = 2.5 * std:: pow(h, 3)  - 8.8 * h + 40.7;
        D = - 75 * std:: pow(h, 3) + 585 * std:: pow(h, 2) + 496 * h - 6.8;
        E = - 169 * std:: pow(h, 4) + 88 * std:: pow(h, 3) + 584 * std:: pow(h, 2) - 39 * h + 0.8;

        beta_s_t = (std:: pow(idade_ficticia / 100, 3) + A * std:: pow(idade_ficticia / 100, 2) + B * idade_ficticia / 100) / 
            (std:: pow(idade_ficticia / 100, 3) + C * std:: pow(idade_ficticia / 100, 2) + D * idade_ficticia / 100 + E);
            
        retracao_concreto_infinito = 1e-4 * ( - 8.09 +(U/15) - (U * U / 2284) - (U * U * U / 133765) + (U * U * U * U / 7608150)) * ( 33 + 2 * h)/ (20.8 + 3*h);

        retracao_concreto_infinito_t0 = retracao_concreto_infinito * (1 - beta_s_t);

    }

    void calcularPerdaPorFluenciaConcreto(Concreto& concreto, ArmaduraAtiva& armadura_ativa, SecaoRetangular& secao, CondicoesClima& condicoes_clima, double dias_ate_ato_protensao){
        double h = secao.espessura_ficticia;
        double U = condicoes_clima.umidade_media;
        t = concreto.alfa_fluencia * (condicoes_clima.temperatura_media + 10) / 30 * dias_ate_ato_protensao;

        fator_fck_j = std:: exp(concreto.s * (1 - std::sqrt( 28 / dias_ate_ato_protensao))) / std::exp(concreto.s);

        A = 42 * std:: pow(h, 3) - 350 * std:: pow(h, 2) + 588 * h + 113;
        B = 768 * std:: pow(h, 3) - 3060 * std:: pow(h, 2) + 3234 * h - 23;
        C = - 200 * std:: pow(h, 3) + 13 * std:: pow(h, 2) + 1090 * h + 183;
        D = 7579 * std:: pow(h, 3) - 31916 * std:: pow(h, 2) + 35343 * h + 1931;

        beta_f_t = ( t * t + A * t + B) / (t * t + C * t + D);

        phi_1c = ( 4.45 - 0.035 * U);
/*
        if( concreto.abatimento_min_cm >= 0.0 && concreto.abatimento_max_cm <= 4.0 ){
            std::vector<std::array<double, 2>> tabela_phi_1c = {
            {100.0, 0.0},
            {90.0, 1.3},
            {70.0, 2.5},
            {40.0, 3.5}};
            
            phi_1c = InterpolarTabela(tabela_phi_1c, U);
        }*/

        if(concreto.fck_MPa >= 20.0 && concreto.fck_MPa <= 45.0 ){
            phi_a = 0.8  * (1 - fator_fck_j);
            phi_f_infinito = phi_1c * (0.42 + h) / (0.20 + h);
        }else if(concreto.fck_MPa >= 50.0 && concreto.fck_MPa <= 90.0){
            phi_a = 1.4  * (1 - fator_fck_j);
            phi_f_infinito = 0.45 * phi_1c * (0.42 + h) / (0.20 + h);
        }else {
            std::cout << "fck além dos valores estipulados de norma para o cálculo de fluência" <<std::endl;
        }

        phi_f_infinito_t0 = phi_f_infinito  * (1 - beta_f_t);

        phi_d = 0.4;

        phi_total = phi_d + phi_a + phi_f_infinito_t0;
    }

};


    Concreto concreto;
    ArmaduraPassiva armadura_passiva;
    SecaoRetangular secao;
    Cabo cabo;
    PerdasProgressivas perdas_progressivas;
    CondicoesClima condicoes_clima;

    double vao; // comprimento da viga
    double MSd; // momento solicitante de cálculo crítico calculado internamente no ELU
    double NSd; // força normal solicitante de cálculo
    double qk; // carga distribuída solicitante característica

    TipoAcaoVariavel tipo_acao_variavel = Acao_Residencias_Hospedagens_Acesso_Restrito;
    TipoCombinacaoServico combinacao_els_f = Combinacao_Frequente;
    TipoCombinacaoServico combinacao_els_d = Combinacao_Quase_Permanente;
    double fator_combinacao_els_f = 0.4;
    double fator_combinacao_els_d = 0.3;

    double gamma_g_elu = 1.4;
    double gamma_q_elu = 1.4;
    double gamma_p_elu = 0.9;
    double maior_momento_solicitante_elu_kNm = 0.0;
    double maior_momento_resistente_elu_kNm = 0.0;
    double maior_As_elu = 0.0;
    int indice_critico_elu = 0;

    double peso_proprio_por_metro = 0.0;
    double peso_proprio_total = 0.0;

    double Pd = 0.0; // força de protensão considerada no ELU
    double e_p = 0.0; // excentricidade considerada no ELU

    double x_linha_neutra = 0.0;
    double x_sobre_d = 0.0;
    double As = 0.0; // área de aço passivo

    double fckj_MPa = 0.0;
    double fctm_j_MPa = 0.0;
    double limite_compressao_ato_MPa = 0.0;
    double limite_tracao_ato_MPa = 0.0;

    double limite_compressao_els_f_MPa = 0.0;
    double limite_tracao_fctf_MPa = 0.0;

    double limite_compressao_els_d_MPa = 0.0;
    double limite_tracao_els_d_MPa = 0.0;

    double maior_perda_progressiva_total_MPa;
    double maior_percentual_perda_total;


    int n_dias_ato_protensao;

    double taxa_de_aco = 0.0;

    bool ato_protensao_aprovado = true;
    bool verificacoes_els_aprovadas = true;
    bool verificacoes_elu_aprovadas = true;
    bool verificacoes_els_f_aprovadas = true;
    bool verificacoes_els_d_aprovadas = true;

    VigaConcretoProtendido(double vao_, double MSd_kNm, double NSd_kN, double qSd_kN_por_m, Concreto concreto_, ArmaduraPassiva armadura_passiva_, SecaoRetangular secao_, Cabo cabo_, CondicoesClima condicoes_clima_, int n_dias_ato_protensao_) :
        concreto(concreto_), armadura_passiva(armadura_passiva_), secao(secao_), cabo(cabo_), condicoes_clima(condicoes_clima_){
        vao = vao_;
        MSd = MSd_kNm * 1000.0;
        NSd = NSd_kN * 1000.0;
        qk = qSd_kN_por_m * 1000.0;
        n_dias_ato_protensao = n_dias_ato_protensao_;
    }

    void dimensionar(){
        prepararDadosIniciais();
        cabo.gerarTracado(vao);
        avaliarPerdas();
        avaliarAtoProtensao();
        avaliarPerdasProgressivas();
        cabo.armadura_ativa.tensao_final = cabo.pontos[0].tensao_apos_perda_progressiva__MPa * 1.0e6;
        cabo.armadura_ativa.forca_final = cabo.pontos[0].forca_apos_perda_progressiva_kN * 1000;

        verificarELS_D();
        verificarELS_F();

        verificarELS();
        verificarELU();
    }

    void prepararDadosIniciais(){
        secao.calcularGeometria(armadura_passiva, condicoes_clima);
        calcularPesoProprio();
        taxa_de_aco = cabo.armadura_ativa.area_total / secao.area;
    }

    void calcularPesoProprio(){
        peso_proprio_por_metro = concreto.peso_especifico * secao.area;
        peso_proprio_total = peso_proprio_por_metro * vao;
    }

    void avaliarPerdas(){
        cabo.avaliarAtrito();
        cabo.avaliarEncunhamento();
        avaliarEncurtamentoElastico();

    }

    void avaliarEncurtamentoElastico(){
        int n = cabo.armadura_ativa.numero_cordoalhas;
        double alfa_p = cabo.armadura_ativa.aco.modulo_elasticidade_MPa / concreto.modulo_elasticidade_inicial;
        double fator_etapas = (double)(n - 1) / (2.0 * (double)n);

        for (int i = 0; i < (int)cabo.pontos.size(); i++){
            cabo.pontos[i].forca_protensao_kN =
                cabo.pontos[i].tensao_apos_encunhamento_MPa
                * 1.0e6
                * cabo.armadura_ativa.area_total
                / 1000.0;

            cabo.pontos[i].excentricidade = (secao.altura /2  - cabo.pontos[i].y);

            cabo.pontos[i].momento_peso_proprio_kNm =
                peso_proprio_por_metro
                * cabo.pontos[i].x
                * (vao - cabo.pontos[i].x)
                / 2.0
                / 1000.0;

            cabo.pontos[i].tensao_concreto_no_nivel_do_cabo_MPa =
                (
                    -(cabo.pontos[i].forca_protensao_kN * 1000.0) / secao.area
                    - (cabo.pontos[i].forca_protensao_kN * 1000.0)
                    * cabo.pontos[i].excentricidade
                    * cabo.pontos[i].excentricidade
                    / secao.inercia_x
                    + (cabo.pontos[i].momento_peso_proprio_kNm * 1000.0)
                    * cabo.pontos[i].excentricidade
                    / secao.inercia_x
                )
                / 1.0e6;

            cabo.pontos[i].perda_encurtamento_MPa =
                alfa_p
                * std::abs(cabo.pontos[i].tensao_concreto_no_nivel_do_cabo_MPa)
                * fator_etapas;

            cabo.pontos[i].tensao_apos_encurtamento_MPa =
                cabo.pontos[i].tensao_apos_encunhamento_MPa
                - cabo.pontos[i].perda_encurtamento_MPa;
        }
    }
    
    void avaliarAtoProtensao(){
        ato_protensao_aprovado = true;

        fckj_MPa = concreto.calcularfckj(n_dias_ato_protensao);
        fctm_j_MPa = concreto.calcularfctmj(n_dias_ato_protensao);

        limite_compressao_ato_MPa = 0.7 * fckj_MPa;
        limite_tracao_ato_MPa = 1.2 * fctm_j_MPa;

        for (int i = 0; i < (int)cabo.pontos.size(); i++){
            cabo.pontos[i].forca_protensao_ato_kN  = 1.1 *
                cabo.pontos[i].tensao_apos_encurtamento_MPa
                * 1.0e6
                * cabo.armadura_ativa.area_total
                / 1000.0;

            cabo.pontos[i].momento_peso_proprio_ato_kNm =
                peso_proprio_por_metro
                * cabo.pontos[i].x
                * (vao - cabo.pontos[i].x)
                / 2.0
                / 1000.0;

            cabo.pontos[i].tensao_topo_ato_MPa =
                (
                    -(cabo.pontos[i].forca_protensao_ato_kN * 1000.0) / secao.area
                    + (cabo.pontos[i].forca_protensao_ato_kN * 1000.0) * cabo.pontos[i].excentricidade * (secao.altura / 2.0) / secao.inercia_x
                    - (cabo.pontos[i].momento_peso_proprio_ato_kNm * 1000.0)* (secao.altura / 2.0)/ secao.inercia_x
                )
                / 1.0e6;

            cabo.pontos[i].tensao_base_ato_MPa =
                (
                    -(cabo.pontos[i].forca_protensao_ato_kN * 1000.0) / secao.area
                    - (cabo.pontos[i].forca_protensao_ato_kN * 1000.0) * cabo.pontos[i].excentricidade * (secao.altura / 2.0) / secao.inercia_x
                    + (cabo.pontos[i].momento_peso_proprio_ato_kNm * 1000.0)* (secao.altura / 2.0)/ secao.inercia_x
                )
                / 1.0e6;

            cabo.pontos[i].limite_compressao_ato_MPa = limite_compressao_ato_MPa;
            cabo.pontos[i].limite_tracao_ato_MPa = limite_tracao_ato_MPa;

            if (cabo.pontos[i].tensao_topo_ato_MPa < -limite_compressao_ato_MPa){
                ato_protensao_aprovado = false;
            }

            if (cabo.pontos[i].tensao_base_ato_MPa < -limite_compressao_ato_MPa){
                ato_protensao_aprovado = false;
            }

            if (cabo.pontos[i].tensao_topo_ato_MPa > limite_tracao_ato_MPa){
                ato_protensao_aprovado = false;
            }

            if (cabo.pontos[i].tensao_base_ato_MPa > limite_tracao_ato_MPa){
                ato_protensao_aprovado = false;
            }
        }
    }

    void avaliarPerdasProgressivas (){
        perdas_progressivas.calcularPerdaPorRetracaoConcreto(concreto, secao, condicoes_clima, n_dias_ato_protensao);
        perdas_progressivas.calcularPerdaPorFluenciaConcreto(concreto, cabo.armadura_ativa, secao, condicoes_clima, n_dias_ato_protensao);

        double taxa_geometrica_aco = cabo.armadura_ativa.area_total / secao.area;
        double alfa_p = cabo.armadura_ativa.aco.modulo_elasticidade_MPa / concreto.modulo_elasticidade_inicial;


        for (int i = 0; i < (int)cabo.pontos.size(); i++){
            perdas_progressivas.calcularPerdaPorRelaxacaoAco(cabo.armadura_ativa, cabo.pontos[i].tensao_apos_encurtamento_MPa);

            cabo.pontos[i].eta = 1 + cabo.pontos[i].excentricidade * cabo.pontos[i].excentricidade * secao.area / secao.inercia_x;

            cabo.pontos[i].tensao_no_concreto_pela_protensao_em_t0 = (cabo.pontos[i].momento_peso_proprio_kNm * 1000 * cabo.pontos[i].excentricidade / secao.inercia_x - cabo.pontos[i].forca_protensao_kN * 1000 * cabo.pontos[i].eta / secao.area) / 1e6;//sigma cp0g

            cabo.pontos[i].perda_progressiva_total_MPa =  (( std:: abs(perdas_progressivas.retracao_concreto_infinito_t0) * 1e4 * cabo.armadura_ativa.aco.modulo_elasticidade_MPa / 1e4 ) + ( alfa_p * ( - cabo.pontos[i].tensao_no_concreto_pela_protensao_em_t0) * perdas_progressivas.phi_total )  + ( cabo.pontos[i].tensao_apos_encurtamento_MPa * perdas_progressivas.coeficiente_fluencia_aco_qui_infinito_percento / 100 ) )  /
        ((1 + perdas_progressivas.coeficiente_fluencia_aco_qui_infinito_percento / 100 ) + ( ( 1 + 0.5 * perdas_progressivas.phi_total ) *cabo.pontos[i].eta * taxa_geometrica_aco * alfa_p));

            cabo.pontos[i].percentual_perda_total = (1 - (cabo.pontos[i].tensao_apos_encurtamento_MPa - cabo.pontos[i].perda_progressiva_total_MPa) / cabo.pontos[i].tensao_apos_atrito_MPa);
        
            cabo.pontos[i].tensao_apos_perda_progressiva__MPa = cabo.pontos[i].tensao_apos_encurtamento_MPa - cabo.pontos[i].perda_progressiva_total_MPa;

            cabo.pontos[i].forca_apos_perda_progressiva_kN = cabo.pontos[i].tensao_apos_perda_progressiva__MPa * 1.0e6 * cabo.armadura_ativa.area_total / 1000.0;
        
        if (i == 0 ){
            maior_perda_progressiva_total_MPa = 0.0;
            maior_percentual_perda_total = 0.0;

        }else{
            maior_perda_progressiva_total_MPa = maiorValor(cabo.pontos[i].perda_progressiva_total_MPa, cabo.pontos[i - 1].perda_progressiva_total_MPa);
            maior_percentual_perda_total = maiorValor(cabo.pontos[i].percentual_perda_total, cabo.pontos[i -1].percentual_perda_total);
        }}
    }

    void verificarELS(){
        verificacoes_els_aprovadas = ato_protensao_aprovado && verificacoes_els_d_aprovadas && verificacoes_els_f_aprovadas;
    }

    // As rotinas especificas antigas do ELU foram removidas.
    // O ELU ponto a ponto passa a reutilizar calcularLinhaNeutra() e calcularAs(),
    // usando MSd calculado somente por Mpp e Mq.

    
    void calcularLinhaNeutra(){
        double d = secao.altura_util;
        double momento_equivalente = MSd - NSd * (d - secao.altura / 2.0) + Pd *(d - e_p - secao.altura / 2.0) / 1.15;
        double termo_raiz = 1.0 - 2.0 * momento_equivalente / (concreto.alfa_c * concreto.eta_c * concreto.fcd * secao.largura * d * d);
        x_linha_neutra = d / concreto.lambda * (1.0 - std::sqrt(termo_raiz));
        x_sobre_d = x_linha_neutra / d;
    }

    void calcularAs(){
        As = ( concreto.alfa_c * concreto.eta_c * concreto.fcd * secao.largura * concreto.lambda* x_linha_neutra
                + NSd - Pd / 1.15)
            / armadura_passiva.aco.fyd;
    }

    void verificarELU(){
        verificacoes_elu_aprovadas = true;
        maior_momento_solicitante_elu_kNm = 0.0;
        maior_momento_resistente_elu_kNm = 0.0;
        maior_As_elu = 0.0;
        indice_critico_elu = 0;

        gamma_g_elu = GammaAcaoPermanente(Efeito_Desfavoravel);
        gamma_q_elu = GammaAcaoVariavel(Efeito_Desfavoravel);
        gamma_p_elu = 0.0;

        for (int i = 0; i < (int)cabo.pontos.size(); i++){
            cabo.pontos[i].altura_util_cabo_elu = secao.altura - cabo.pontos[i].y;
            cabo.pontos[i].excentricidade_elu = cabo.pontos[i].y - secao.altura / 2.0;

            if (cabo.pontos[i].altura_util_cabo_elu > 0.0) {
                cabo.pontos[i].taxa_geometrica_elu = cabo.armadura_ativa.area_total / (secao.largura * cabo.pontos[i].altura_util_cabo_elu);
            }
            else {
                cabo.pontos[i].taxa_geometrica_elu = 0.0;
            }

            cabo.pontos[i].relacao_vao_altura_util_elu = 0.0;
            cabo.pontos[i].limite_delta_sigma_p_elu_MPa = 0.0;

            if (cabo.pontos[i].taxa_geometrica_elu > 0.0 && cabo.pontos[i].altura_util_cabo_elu > 0.0) {
                cabo.pontos[i].relacao_vao_altura_util_elu = vao / cabo.pontos[i].altura_util_cabo_elu;

                double denominador_rho = 100.0;
                double limite_delta_sigma_p_MPa = 420.0;

                if (cabo.pontos[i].relacao_vao_altura_util_elu > 35.0) {
                    denominador_rho = 300.0;
                    limite_delta_sigma_p_MPa = 210.0;
                }

                cabo.pontos[i].delta_sigma_p_elu_MPa =
                    70.0
                    + concreto.fck_MPa
                    / (denominador_rho * cabo.pontos[i].taxa_geometrica_elu);

                if (cabo.pontos[i].delta_sigma_p_elu_MPa > limite_delta_sigma_p_MPa) {
                    cabo.pontos[i].delta_sigma_p_elu_MPa = limite_delta_sigma_p_MPa;
                }

                cabo.pontos[i].limite_delta_sigma_p_elu_MPa = limite_delta_sigma_p_MPa;
            }
            else {
                cabo.pontos[i].delta_sigma_p_elu_MPa = 0.0;
            }

            cabo.pontos[i].tensao_protensao_elu_MPa = cabo.pontos[i].tensao_apos_perda_progressiva__MPa + cabo.pontos[i].delta_sigma_p_elu_MPa;

            if (cabo.pontos[i].tensao_protensao_elu_MPa > cabo.armadura_ativa.aco.fpyk / 1.0e6) {
                cabo.pontos[i].tensao_protensao_elu_MPa = cabo.armadura_ativa.aco.fpyk / 1.0e6;
                cabo.pontos[i].delta_sigma_p_elu_MPa = cabo.pontos[i].tensao_protensao_elu_MPa - cabo.pontos[i].tensao_apos_perda_progressiva__MPa;
            }

            cabo.pontos[i].forca_protensao_elu_kN =
                cabo.pontos[i].tensao_protensao_elu_MPa
                * 1.0e6
                * cabo.armadura_ativa.area_total
                / 1000.0;

            cabo.pontos[i].momento_peso_proprio_elu_kNm = cabo.pontos[i].momento_peso_proprio_kNm;

            cabo.pontos[i].momento_carga_elu_kNm =
                qk
                * cabo.pontos[i].x
                * (vao - cabo.pontos[i].x)
                / 2.0
                / 1000.0;


            cabo.pontos[i].momento_solicitante_elu_kNm =
                cabo.pontos[i].momento_peso_proprio_elu_kNm * gamma_g_elu
                + cabo.pontos[i].momento_carga_elu_kNm * gamma_q_elu;

            MSd = cabo.pontos[i].momento_solicitante_elu_kNm * 1000.0;

            Pd = cabo.pontos[i].forca_protensao_elu_kN * 1000.0;
            e_p = std::abs(cabo.pontos[i].excentricidade_elu) ;

            calcularLinhaNeutra();
            calcularAs();

            if (std::isfinite(As) && As < 0.0) {
                As = 0.0;
            }

            cabo.pontos[i].x_linha_neutra_elu = x_linha_neutra;
            cabo.pontos[i].x_sobre_d_elu = x_sobre_d;
            cabo.pontos[i].As_elu = As;
            cabo.pontos[i].momento_resistente_elu_kNm = cabo.pontos[i].momento_solicitante_elu_kNm;

            cabo.pontos[i].elu_aprovado_no_ponto =
                std::isfinite(cabo.pontos[i].x_linha_neutra_elu)
                && std::isfinite(cabo.pontos[i].As_elu);

            if (!cabo.pontos[i].elu_aprovado_no_ponto) {
                verificacoes_elu_aprovadas = false;
            }

            if (cabo.pontos[i].momento_solicitante_elu_kNm > maior_momento_solicitante_elu_kNm) {
                maior_momento_solicitante_elu_kNm = cabo.pontos[i].momento_solicitante_elu_kNm;
                maior_momento_resistente_elu_kNm = cabo.pontos[i].momento_resistente_elu_kNm;
            }

            if (cabo.pontos[i].elu_aprovado_no_ponto && cabo.pontos[i].As_elu > maior_As_elu) {
                maior_As_elu = cabo.pontos[i].As_elu;
                indice_critico_elu = i;
            }
        }

        if ((int)cabo.pontos.size() > 0) {
            MSd = cabo.pontos[indice_critico_elu].momento_solicitante_elu_kNm * 1000.0;
            Pd = cabo.pontos[indice_critico_elu].forca_protensao_elu_kN * 1000.0;
            e_p = std::abs(cabo.pontos[indice_critico_elu].excentricidade_elu);

            calcularLinhaNeutra();
            calcularAs();

            if (std::isfinite(As) && As < 0.0) {
                As = 0.0;
            }
        }
    }

    void verificarELS_F(){
        verificacoes_els_f_aprovadas = true;

        combinacao_els_f = CombinacaoELSFPorNivelProtensao(cabo.armadura_ativa.nivel_protensao);
        fator_combinacao_els_f = FatorCombinacaoServico(combinacao_els_f, tipo_acao_variavel);

        limite_compressao_els_f_MPa = 0.60 * concreto.fck_MPa;
        limite_tracao_fctf_MPa = 0.3 * std::pow(concreto.fck_MPa, 2.0 / 3.0);

        for (int i = 0; i < (int)cabo.pontos.size(); i++){
            cabo.pontos[i].tensao_els_f_MPa = cabo.pontos[i].tensao_apos_perda_progressiva__MPa;
            cabo.pontos[i].forca_protensao_els_f_kN = cabo.pontos[i].forca_apos_perda_progressiva_kN;
            cabo.pontos[i].momento_peso_proprio_kNm;

            cabo.pontos[i].momento_devido_carga_kNm = 
                qk * cabo.pontos[i].x * vao / 2.0
                / 1000.0
                -(qk * cabo.pontos[i].x * cabo.pontos[i].x) / 2.0
                 /1000.0;

            cabo.pontos[i].momento_els_f_kNm = cabo.pontos[i].momento_peso_proprio_kNm + cabo.pontos[i].momento_devido_carga_kNm * fator_combinacao_els_f;

            cabo.pontos[i].tensao_topo_els_f_MPa =
                (
                    -(cabo.pontos[i].forca_protensao_els_f_kN * 1000.0) / secao.area
                    + (cabo.pontos[i].forca_protensao_els_f_kN * 1000.0) * cabo.pontos[i].excentricidade * (secao.altura / 2.0)/ secao.inercia_x
                    - (cabo.pontos[i].momento_els_f_kNm * 1000.0) * (secao.altura / 2.0)/ secao.inercia_x

                )
                / 1.0e6;
                
            cabo.pontos[i].tensao_base_els_f_MPa =
                (
                    -(cabo.pontos[i].forca_protensao_els_f_kN * 1000.0) / secao.area
                    - (cabo.pontos[i].forca_protensao_els_f_kN * 1000.0) * cabo.pontos[i].excentricidade * (secao.altura / 2.0)/ secao.inercia_x
                    + (cabo.pontos[i].momento_els_f_kNm * 1000.0) * (secao.altura / 2.0) / secao.inercia_x
                )
                / 1.0e6;

            cabo.pontos[i].limite_compressao_els_f_MPa = limite_compressao_els_f_MPa;
            cabo.pontos[i].limite_tracao_fctf_MPa = limite_tracao_fctf_MPa;

            if (cabo.pontos[i].tensao_topo_els_f_MPa < -limite_compressao_els_f_MPa){
                verificacoes_els_f_aprovadas = false;
            }

            if (cabo.pontos[i].tensao_base_els_f_MPa < -limite_compressao_els_f_MPa){
                verificacoes_els_f_aprovadas = false;
            }

            if (cabo.pontos[i].tensao_topo_els_f_MPa > limite_tracao_fctf_MPa){
                verificacoes_els_f_aprovadas = false;
            }

            if (cabo.pontos[i].tensao_base_els_f_MPa > limite_tracao_fctf_MPa){
                verificacoes_els_f_aprovadas = false;
            }
        }

    }

    void verificarELS_D(){
        verificacoes_els_d_aprovadas = true;
        
        combinacao_els_d = CombinacaoELSDPorNivelProtensao(cabo.armadura_ativa.nivel_protensao);
        fator_combinacao_els_d = FatorCombinacaoServico(combinacao_els_d, tipo_acao_variavel);

        limite_compressao_els_d_MPa = 0.45 * concreto.fck_MPa;
        limite_tracao_els_d_MPa = 0.0;

        for (int i = 0; i < (int)cabo.pontos.size(); i++){
            cabo.pontos[i].tensao_els_d_MPa = cabo.pontos[i].tensao_apos_perda_progressiva__MPa;
            cabo.pontos[i].forca_protensao_els_d_kN = cabo.pontos[i].forca_apos_perda_progressiva_kN;
            cabo.pontos[i].momento_peso_proprio_kNm;

            cabo.pontos[i].momento_devido_carga_kNm = 
                qk * cabo.pontos[i].x * vao / 2.0
                / 1000.0
                -(qk * cabo.pontos[i].x * cabo.pontos[i].x) / 2.0
                 /1000.0;

            cabo.pontos[i].momento_els_d_kNm = cabo.pontos[i].momento_peso_proprio_kNm + cabo.pontos[i].momento_devido_carga_kNm * fator_combinacao_els_d;

            cabo.pontos[i].tensao_topo_els_d_MPa =
                (
                    -(cabo.pontos[i].forca_protensao_els_d_kN * 1000.0) / secao.area
                    + (cabo.pontos[i].forca_protensao_els_d_kN * 1000.0)
                    * cabo.pontos[i].excentricidade
                    * (secao.altura / 2.0)
                    / secao.inercia_x
                    - (cabo.pontos[i].momento_els_d_kNm * 1000.0)
                    * (secao.altura / 2.0)
                    / secao.inercia_x
                )
                / 1.0e6;

            cabo.pontos[i].tensao_base_els_d_MPa =
                (
                    -(cabo.pontos[i].forca_protensao_els_d_kN * 1000.0) / secao.area
                    - (cabo.pontos[i].forca_protensao_els_d_kN * 1000.0)
                    * cabo.pontos[i].excentricidade
                    * (secao.altura / 2.0)
                    / secao.inercia_x
                    + (cabo.pontos[i].momento_els_d_kNm * 1000.0)
                    * (secao.altura / 2.0)
                    / secao.inercia_x
                )
                / 1.0e6;

            cabo.pontos[i].limite_compressao_els_d_MPa = limite_compressao_els_d_MPa;
            cabo.pontos[i].limite_tracao_els_d_MPa = limite_tracao_els_d_MPa;

            if (cabo.pontos[i].tensao_topo_els_d_MPa< -limite_compressao_els_d_MPa){
                verificacoes_els_d_aprovadas = false;
            }

            if (cabo.pontos[i].tensao_base_els_d_MPa < -limite_compressao_els_d_MPa){
                verificacoes_els_d_aprovadas = false;
            }

            if (cabo.pontos[i].tensao_topo_els_d_MPa > limite_tracao_els_d_MPa){
                verificacoes_els_d_aprovadas = false;
            }

            if (cabo.pontos[i].tensao_base_els_d_MPa > limite_tracao_els_d_MPa){
                verificacoes_els_d_aprovadas = false;
            }
        }

    }

    void definirForcaDeProtensaoParaDimensionamento(){
        Pd = cabo.pontos[indice_critico_elu].forca_protensao_elu_kN * 1000.0;
        e_p = (cabo.pontos[indice_critico_elu].excentricidade_elu);
    }


    /*void calcularLinhaNeutra(){
        x_linha_neutra = cabo.pontos[indice_critico_elu].x_linha_neutra_elu;
        x_sobre_d = cabo.pontos[indice_critico_elu].x_sobre_d_elu;
    }

    void calcularAs(){
        As = maior_As_elu;
    }*/
};


double CalcularAsMinimaGraficaCm2(const VigaConcretoProtendido& viga)
{
    // Referência gráfica: 0,15% da área bruta da seção.
    // Este valor não interfere no dimensionamento nem no status do ELU.
    return 0.0015 * viga.secao.area * 1.0e4;
}

struct Graficos{
    double menorValor(double a, double b)
    {
        return a < b ? a : b;
    }

    double maiorValor(double a, double b)
    {
        return a > b ? a : b;
    }

    std::string nomeTrecho(int indice)
    {
        if (indice == 0) {
            return "0";
        }

        return std::to_string(indice - 1) + "-" + std::to_string(indice);
    }

    double passoMalhaX(double vao)
    {
        double passo = std::round(vao / 20.0);

        if (passo < 1.0) {
            passo = 1.0;
        }

        return passo;
    }

    void prepararLimitesInteirosY(double& ymin, double& ymax, bool ancorar_zero)
    {
        ymin = std::floor(ymin);
        ymax = std::ceil(ymax);

        if (ymax <= ymin) {
            ymax = ymin + 1.0;
        }

        double passo = std::ceil((ymax - ymin) / 15.0);

        if (passo < 1.0) {
            passo = 1.0;
        }

        if (ancorar_zero && ymin >= 0.0) {
            ymin = 0.0;
            ymax = 15.0 * passo;

            while (ymax < std::ceil(ymax)) {
                ymax += passo;
            }
        }
        else {
            double intervalo_alvo = 15.0 * passo;
            double sobra = intervalo_alvo - (ymax - ymin);
            ymin = ymin - std::floor(sobra / 2.0);
            ymax = ymin + intervalo_alvo;
        }
    }

    void configurarEixosGrafico(
        double vao,
        const char* titulo_x,
        const char* titulo_y,
        double ymin,
        double ymax,
        bool inverter_y = false,
        bool ancorar_zero = false
    )
    {
        double passo_x = passoMalhaX(vao);
        double xmax = std::ceil(vao / passo_x) * passo_x;

        if (xmax < passo_x) {
            xmax = passo_x;
        }

        prepararLimitesInteirosY(ymin, ymax, ancorar_zero);

        ImPlotAxisFlags flags_y = inverter_y
            ? ImPlotAxisFlags_Invert
            : ImPlotAxisFlags_None;

        ImPlot::SetupAxis(ImAxis_X1, titulo_x);
        ImPlot::SetupAxis(ImAxis_Y1, titulo_y, flags_y);
        ImPlot::SetupAxisLimits(ImAxis_X1, 0.0, xmax, ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, ymin, ymax, ImGuiCond_Always);

        int divisoes_x = (int)std::round(xmax / passo_x);

        ImPlot::SetupAxisTicks(ImAxis_X1, 0.0, xmax, divisoes_x + 1);
        ImPlot::SetupAxisTicks(ImAxis_Y1, ymin, ymax, 16);
        ImPlot::SetupAxisFormat(ImAxis_X1, "%.0f");
        ImPlot::SetupAxisFormat(ImAxis_Y1, "%.0f");

        ImPlot::SetupLegend(
            ImPlotLocation_North,
            ImPlotLegendFlags_Outside | ImPlotLegendFlags_Horizontal
        );
    }

    std::string textoComCasas(double valor, int casas)
    {
        std::ostringstream texto;
        texto << std::fixed << std::setprecision(casas) << valor;
        return texto.str();
    }

    void desenharSeta(ImDrawList* draw, ImVec2 p1, ImVec2 p2, ImU32 cor, float espessura)
    {
        draw->AddLine(p1, p2, cor, espessura);

        float angulo = atan2(p2.y - p1.y, p2.x - p1.x);
        float tamanho = 8.0f;

        ImVec2 a;
        a.x = p2.x - tamanho * cos(angulo - 0.60f);
        a.y = p2.y - tamanho * sin(angulo - 0.60f);

        ImVec2 b;
        b.x = p2.x - tamanho * cos(angulo + 0.60f);
        b.y = p2.y - tamanho * sin(angulo + 0.60f);

        draw->AddLine(p2, a, cor, espessura);
        draw->AddLine(p2, b, cor, espessura);
    }

    void desenharLinhaPontilhadaVertical(
        ImDrawList* draw,
        float x,
        float y_inicio,
        float y_fim,
        ImU32 cor,
        float espessura
    )
    {
        const float comprimento_traco = 5.0f;
        const float espacamento_tracos = 4.0f;

        for (
            float y = y_inicio;
            y < y_fim;
            y += comprimento_traco + espacamento_tracos
        ) {
            float y_final_traco = y + comprimento_traco;

            if (y_final_traco > y_fim) {
                y_final_traco = y_fim;
            }

            draw->AddLine(
                ImVec2(x, y),
                ImVec2(x, y_final_traco),
                cor,
                espessura
            );
        }
    }

    void desenharEsquemaVigaBiapoiada(VigaConcretoProtendido& viga, float largura_canvas, float altura_canvas)
    {
        ImGui::InvisibleButton("canvas_esquema_viga", ImVec2(largura_canvas, altura_canvas));

        ImVec2 origem = ImGui::GetItemRectMin();
        ImDrawList* draw = ImGui::GetWindowDrawList();

        ImU32 cor_linha = IM_COL32(20, 20, 20, 255);
        ImU32 cor_concreto = IM_COL32(220, 220, 220, 255);
        ImU32 cor_carga = IM_COL32(30, 30, 30, 255);
        ImU32 cor_cabo = IM_COL32(190, 30, 30, 255);
        ImU32 cor_texto = IM_COL32(20, 20, 20, 255);
        ImU32 cor_normal = IM_COL32(40, 90, 180, 255);
        ImU32 cor_divisao = IM_COL32(90, 90, 90, 190);

        float margem_esquerda = 80.0f;
        float margem_direita = 160.0f;
        float y_viga = origem.y + 105.0f;
        float altura_viga_px = 26.0f;
        float y_base_viga = y_viga + altura_viga_px;

        if (largura_canvas < 420.0f) {
            margem_esquerda = 55.0f;
            margem_direita = 115.0f;
        }

        float x_inicio = origem.x + margem_esquerda;
        float x_fim = origem.x + largura_canvas - margem_direita;
        float comprimento_px = x_fim - x_inicio;

        if (comprimento_px < 120.0f) {
            return;
        }

        draw->AddRectFilled(
            ImVec2(x_inicio, y_viga),
            ImVec2(x_fim, y_base_viga),
            cor_concreto
        );

        draw->AddRect(
            ImVec2(x_inicio, y_viga),
            ImVec2(x_fim, y_base_viga),
            cor_linha,
            0.0f,
            0,
            2.0f
        );

        if ((int)viga.cabo.pontos.size() > 2 && viga.vao > 0.0) {
            for (int i = 1; i < (int)viga.cabo.pontos.size() - 1; i++) {
                float x_divisao =
                    x_inicio
                    + (float)(viga.cabo.pontos[i].x / viga.vao)
                    * comprimento_px;

                desenharLinhaPontilhadaVertical(
                    draw,
                    x_divisao,
                    y_viga,
                    y_base_viga,
                    cor_divisao,
                    1.0f
                );
            }
        }

        if ((int)viga.cabo.pontos.size() >= 2 && viga.secao.altura > 0.0 && viga.vao > 0.0) {
            for (int i = 0; i < (int)viga.cabo.pontos.size() - 1; i++) {
                float x1 = x_inicio + (float)(viga.cabo.pontos[i].x / viga.vao) * comprimento_px;
                float x2 = x_inicio + (float)(viga.cabo.pontos[i + 1].x / viga.vao) * comprimento_px;

                float y1 = y_base_viga - (float)(viga.cabo.pontos[i].y / viga.secao.altura) * altura_viga_px;
                float y2 = y_base_viga - (float)(viga.cabo.pontos[i + 1].y / viga.secao.altura) * altura_viga_px;

                draw->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), cor_cabo, 2.0f);
            }
        }

        float y_carga = y_viga - 58.0f;

        draw->AddLine(
            ImVec2(x_inicio, y_carga),
            ImVec2(x_fim, y_carga),
            cor_linha,
            2.0f
        );

        int numero_setas = 12;

        for (int i = 0; i <= numero_setas; i++) {
            float t = (float)i / (float)numero_setas;
            float x = x_inicio + t * comprimento_px;

            desenharSeta(
                draw,
                ImVec2(x, y_carga),
                ImVec2(x, y_viga - 4.0f),
                cor_carga,
                2.0f
            );
        }

        std::string texto_q = "qk = " + textoComCasas(viga.qk / 1000.0, 2) + " kN/m";

        draw->AddText(
            ImVec2((x_inicio + x_fim) / 2.0f - 55.0f, y_carga - 26.0f),
            cor_texto,
            texto_q.c_str()
        );

        if (std::abs(viga.NSd) > 1.0e-9) {
            float y_normal = y_viga + altura_viga_px * 0.5f;
            float comprimento_seta_normal = 48.0f;
            std::string texto_n = "Nsd = " + textoComCasas(viga.NSd / 1000.0, 2) + " kN";

            if (viga.NSd > 0.0) {
                desenharSeta(
                    draw,
                    ImVec2(x_inicio, y_normal),
                    ImVec2(x_inicio - comprimento_seta_normal, y_normal),
                    cor_normal,
                    2.0f
                );

                desenharSeta(
                    draw,
                    ImVec2(x_fim, y_normal),
                    ImVec2(x_fim + comprimento_seta_normal, y_normal),
                    cor_normal,
                    2.0f
                );
            }
            else {
                desenharSeta(
                    draw,
                    ImVec2(x_inicio - comprimento_seta_normal, y_normal),
                    ImVec2(x_inicio, y_normal),
                    cor_normal,
                    2.0f
                );

                desenharSeta(
                    draw,
                    ImVec2(x_fim + comprimento_seta_normal, y_normal),
                    ImVec2(x_fim, y_normal),
                    cor_normal,
                    2.0f
                );
            }

            draw->AddText(
                ImVec2(x_fim + 50.0f, y_normal - 8.0f),
                cor_normal,
                texto_n.c_str()
            );
        }

        ImVec2 apoio_esq_p1(x_inicio, y_base_viga);
        ImVec2 apoio_esq_p2(x_inicio - 18.0f, y_base_viga + 28.0f);
        ImVec2 apoio_esq_p3(x_inicio + 18.0f, y_base_viga + 28.0f);

        draw->AddTriangle(apoio_esq_p1, apoio_esq_p2, apoio_esq_p3, cor_linha, 2.0f);
        draw->AddLine(
            ImVec2(x_inicio - 30.0f, y_base_viga + 28.0f),
            ImVec2(x_inicio + 30.0f, y_base_viga + 28.0f),
            cor_linha,
            2.0f
        );

        ImVec2 apoio_dir_p1(x_fim, y_base_viga);
        ImVec2 apoio_dir_p2(x_fim - 18.0f, y_base_viga + 28.0f);
        ImVec2 apoio_dir_p3(x_fim + 18.0f, y_base_viga + 28.0f);

        draw->AddTriangle(apoio_dir_p1, apoio_dir_p2, apoio_dir_p3, cor_linha, 2.0f);
        draw->AddLine(
            ImVec2(x_fim - 30.0f, y_base_viga + 28.0f),
            ImVec2(x_fim + 30.0f, y_base_viga + 28.0f),
            cor_linha,
            2.0f
        );

        float y_cota_l = y_base_viga + 62.0f;

        draw->AddLine(ImVec2(x_inicio, y_cota_l), ImVec2(x_fim, y_cota_l), cor_linha, 1.5f);
        draw->AddLine(ImVec2(x_inicio, y_cota_l - 6.0f), ImVec2(x_inicio, y_cota_l + 6.0f), cor_linha, 1.5f);
        draw->AddLine(ImVec2(x_fim, y_cota_l - 6.0f), ImVec2(x_fim, y_cota_l + 6.0f), cor_linha, 1.5f);

        std::string texto_l = "L = " + textoComCasas(viga.vao, 2) + " m";

        draw->AddText(
            ImVec2((x_inicio + x_fim) / 2.0f - 35.0f, y_cota_l + 8.0f),
            cor_texto,
            texto_l.c_str()
        );
    }


    void desenharSecaoRetangular(VigaConcretoProtendido& viga, float largura_canvas, float altura_canvas)
    {
        ImGui::InvisibleButton("canvas_secao_retangular", ImVec2(largura_canvas, altura_canvas));

        ImVec2 origem = ImGui::GetItemRectMin();
        ImDrawList* draw = ImGui::GetWindowDrawList();

        ImU32 cor_linha = IM_COL32(20, 20, 20, 255);
        ImU32 cor_concreto = IM_COL32(220, 220, 220, 255);
        ImU32 cor_cabo = IM_COL32(190, 30, 30, 255);
        ImU32 cor_texto = IM_COL32(20, 20, 20, 255);

        float h_m = (float)viga.secao.altura;
        float b_m = (float)viga.secao.largura;

        if (h_m <= 0.0f || b_m <= 0.0f) {
            return;
        }

        float margem_esquerda = 25.0f;
        float margem_direita = 175.0f;
        float margem_superior = 30.0f;
        float margem_inferior = 70.0f;

        float largura_util = largura_canvas - margem_esquerda - margem_direita;
        float altura_util = altura_canvas - margem_superior - margem_inferior;

        if (largura_util < 60.0f || altura_util < 80.0f) {
            return;
        }

        float escala_h = altura_util / h_m;
        float escala_b = largura_util / b_m;
        float escala = escala_h;

        if (escala_b < escala_h) {
            escala = escala_b;
        }

        float b_px = b_m * escala;
        float h_px = h_m * escala;

        float x_centro = origem.x + margem_esquerda + largura_util / 2.0f;
        float y_topo = origem.y + margem_superior;

        float x1 = x_centro - b_px / 2.0f;
        float x2 = x_centro + b_px / 2.0f;
        float y1 = y_topo;
        float y2 = y_topo + h_px;

        draw->AddRectFilled(ImVec2(x1, y1), ImVec2(x2, y2), cor_concreto);
        draw->AddRect(ImVec2(x1, y1), ImVec2(x2, y2), cor_linha, 0.0f, 0, 2.0f);

        double altura_cabo_meio = viga.cabo.altura_do_cabo_no_apoio;

        if ((int)viga.cabo.pontos.size() >= 2 && viga.vao > 0.0) {
            double x_meio = viga.vao / 2.0;
            altura_cabo_meio = viga.cabo.pontos[0].y;

            for (int i = 0; i < (int)viga.cabo.pontos.size() - 1; i++) {
                double x_inicial = viga.cabo.pontos[i].x;
                double x_final = viga.cabo.pontos[i + 1].x;

                if (x_meio >= x_inicial && x_meio <= x_final) {
                    double fator = (x_meio - x_inicial) / (x_final - x_inicial);
                    altura_cabo_meio = viga.cabo.pontos[i].y + fator * (viga.cabo.pontos[i + 1].y - viga.cabo.pontos[i].y);
                    break;
                }
            }
        }

        if (altura_cabo_meio < 0.0) {
            altura_cabo_meio = 0.0;
        }

        if (altura_cabo_meio > viga.secao.altura) {
            altura_cabo_meio = viga.secao.altura;
        }

        float y_cabo = y2 - (float)altura_cabo_meio * escala;

        draw->AddCircleFilled(ImVec2(x_centro, y_cabo), 5.0f, cor_cabo);
        draw->AddText(ImVec2(x_centro + 10.0f, y_cabo - 8.0f), cor_texto, "cabo");

        float x_cota_h = x2 + 28.0f;

        draw->AddLine(ImVec2(x_cota_h, y1), ImVec2(x_cota_h, y2), cor_linha, 1.5f);
        draw->AddLine(ImVec2(x_cota_h - 6.0f, y1), ImVec2(x_cota_h + 6.0f, y1), cor_linha, 1.5f);
        draw->AddLine(ImVec2(x_cota_h - 6.0f, y2), ImVec2(x_cota_h + 6.0f, y2), cor_linha, 1.5f);

        std::string texto_h = textoComCasas(viga.secao.altura * 100.0, 0) + " cm";

        draw->AddText(
            ImVec2(x_cota_h + 8.0f, (y1 + y2) / 2.0f - 8.0f),
            cor_texto,
            texto_h.c_str()
        );

        float y_cota_b = y2 + 28.0f;

        draw->AddLine(ImVec2(x1, y_cota_b), ImVec2(x2, y_cota_b), cor_linha, 1.5f);
        draw->AddLine(ImVec2(x1, y_cota_b - 6.0f), ImVec2(x1, y_cota_b + 6.0f), cor_linha, 1.5f);
        draw->AddLine(ImVec2(x2, y_cota_b - 6.0f), ImVec2(x2, y_cota_b + 6.0f), cor_linha, 1.5f);

        std::string texto_b = textoComCasas(viga.secao.largura * 100.0, 0) + " cm";

        draw->AddText(
            ImVec2((x1 + x2) / 2.0f - 18.0f, y_cota_b + 8.0f),
            cor_texto,
            texto_b.c_str()
        );
    }


    void desenharTracadoCompletoCabo(VigaConcretoProtendido& viga, float largura_canvas, float altura_canvas)
    {
        ImGui::InvisibleButton("canvas_tracado_completo_cabo", ImVec2(largura_canvas, altura_canvas));

        ImVec2 origem = ImGui::GetItemRectMin();
        ImDrawList* draw = ImGui::GetWindowDrawList();

        ImU32 cor_linha = IM_COL32(20, 20, 20, 255);
        ImU32 cor_concreto = IM_COL32(225, 225, 225, 255);
        ImU32 cor_cabo = IM_COL32(190, 30, 30, 255);
        ImU32 cor_texto = IM_COL32(20, 20, 20, 255);
        ImU32 cor_cota = IM_COL32(40, 40, 40, 255);
        ImU32 cor_divisao = IM_COL32(90, 90, 90, 190);

        if (viga.secao.altura <= 0.0 || viga.vao <= 0.0 || (int)viga.cabo.pontos.size() < 2) {
            return;
        }

        float margem_esquerda = 70.0f;
        float margem_direita = 45.0f;
        float margem_superior = 32.0f;
        float margem_inferior = 70.0f;

        float x_inicio = origem.x + margem_esquerda;
        float x_fim = origem.x + largura_canvas - margem_direita;
        float comprimento_px = x_fim - x_inicio;

        if (comprimento_px < 180.0f) {
            return;
        }

        float altura_viga_px = 84.0f;
        float altura_disponivel = altura_canvas - margem_superior - margem_inferior;

        if (altura_viga_px > altura_disponivel) {
            altura_viga_px = altura_disponivel;
        }

        if (altura_viga_px < 55.0f) {
            altura_viga_px = 55.0f;
        }

        float y_viga = origem.y + margem_superior + 12.0f;
        float y_base_viga = y_viga + altura_viga_px;

        draw->AddRectFilled(
            ImVec2(x_inicio, y_viga),
            ImVec2(x_fim, y_base_viga),
            cor_concreto
        );

        draw->AddRect(
            ImVec2(x_inicio, y_viga),
            ImVec2(x_fim, y_base_viga),
            cor_linha,
            0.0f,
            0,
            2.0f
        );

        for (int i = 1; i < (int)viga.cabo.pontos.size() - 1; i++) {
            float x_divisao =
                x_inicio
                + (float)(viga.cabo.pontos[i].x / viga.vao)
                * comprimento_px;

            desenharLinhaPontilhadaVertical(
                draw,
                x_divisao,
                y_viga,
                y_base_viga,
                cor_divisao,
                1.0f
            );
        }

        for (int i = 0; i < (int)viga.cabo.pontos.size() - 1; i++) {
            float x1 = x_inicio + (float)(viga.cabo.pontos[i].x / viga.vao) * comprimento_px;
            float x2 = x_inicio + (float)(viga.cabo.pontos[i + 1].x / viga.vao) * comprimento_px;

            float y1 = y_base_viga - (float)(viga.cabo.pontos[i].y / viga.secao.altura) * altura_viga_px;
            float y2 = y_base_viga - (float)(viga.cabo.pontos[i + 1].y / viga.secao.altura) * altura_viga_px;

            draw->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), cor_cabo, 2.5f);
        }

        double altura_cabo_apoio = viga.cabo.pontos[0].y;
        double altura_cabo_centro = viga.cabo.pontos[0].y;
        double x_meio = viga.vao / 2.0;

        for (int i = 0; i < (int)viga.cabo.pontos.size() - 1; i++) {
            double x_inicial = viga.cabo.pontos[i].x;
            double x_final = viga.cabo.pontos[i + 1].x;

            if (x_meio >= x_inicial && x_meio <= x_final) {
                double fator = (x_meio - x_inicial) / (x_final - x_inicial);
                altura_cabo_centro = viga.cabo.pontos[i].y + fator * (viga.cabo.pontos[i + 1].y - viga.cabo.pontos[i].y);
                break;
            }
        }

        if (altura_cabo_apoio < 0.0) {
            altura_cabo_apoio = 0.0;
        }

        if (altura_cabo_apoio > viga.secao.altura) {
            altura_cabo_apoio = viga.secao.altura;
        }

        if (altura_cabo_centro < 0.0) {
            altura_cabo_centro = 0.0;
        }

        if (altura_cabo_centro > viga.secao.altura) {
            altura_cabo_centro = viga.secao.altura;
        }

        float y_cabo_apoio = y_base_viga - (float)(altura_cabo_apoio / viga.secao.altura) * altura_viga_px;
        float y_cabo_centro = y_base_viga - (float)(altura_cabo_centro / viga.secao.altura) * altura_viga_px;
        float x_centro = x_inicio + 0.5f * comprimento_px;

        float x_cota_ha = x_inicio + 28.0f;
        draw->AddLine(ImVec2(x_cota_ha, y_base_viga), ImVec2(x_cota_ha, y_cabo_apoio), cor_cota, 1.5f);
        draw->AddLine(ImVec2(x_cota_ha - 5.0f, y_base_viga), ImVec2(x_cota_ha + 5.0f, y_base_viga), cor_cota, 1.5f);
        draw->AddLine(ImVec2(x_cota_ha - 5.0f, y_cabo_apoio), ImVec2(x_cota_ha + 5.0f, y_cabo_apoio), cor_cota, 1.5f);

        std::string texto_ha = "Ha = " + textoComCasas(altura_cabo_apoio * 100.0, 1) + " cm";
        draw->AddText(ImVec2(x_cota_ha + 8.0f, (y_base_viga + y_cabo_apoio) / 2.0f - 8.0f), cor_texto, texto_ha.c_str());

        float x_cota_hc = x_centro + 36.0f;
        draw->AddLine(ImVec2(x_cota_hc, y_base_viga), ImVec2(x_cota_hc, y_cabo_centro), cor_cota, 1.5f);
        draw->AddLine(ImVec2(x_cota_hc - 5.0f, y_base_viga), ImVec2(x_cota_hc + 5.0f, y_base_viga), cor_cota, 1.5f);
        draw->AddLine(ImVec2(x_cota_hc - 5.0f, y_cabo_centro), ImVec2(x_cota_hc + 5.0f, y_cabo_centro), cor_cota, 1.5f);

        std::string texto_hc = "Hc = " + textoComCasas(altura_cabo_centro * 100.0, 1) + " cm";
        draw->AddText(ImVec2(x_cota_hc + 8.0f, (y_base_viga + y_cabo_centro) / 2.0f - 8.0f), cor_texto, texto_hc.c_str());

        float y_cota_l = y_base_viga + 38.0f;

        draw->AddLine(ImVec2(x_inicio, y_cota_l), ImVec2(x_fim, y_cota_l), cor_linha, 1.5f);
        draw->AddLine(ImVec2(x_inicio, y_cota_l - 6.0f), ImVec2(x_inicio, y_cota_l + 6.0f), cor_linha, 1.5f);
        draw->AddLine(ImVec2(x_fim, y_cota_l - 6.0f), ImVec2(x_fim, y_cota_l + 6.0f), cor_linha, 1.5f);

        std::string texto_l = "L = " + textoComCasas(viga.vao, 2) + " m";

        draw->AddText(
            ImVec2((x_inicio + x_fim) / 2.0f - 35.0f, y_cota_l + 8.0f),
            cor_texto,
            texto_l.c_str()
        );
    }

    void desenharEsquemasDaViga(VigaConcretoProtendido& viga)
    {
        ImGui::Separator();
        ImGui::Text("Esquemas da viga:");

        float largura_total = ImGui::GetContentRegionAvail().x;
        float espacamento = 20.0f;
        float altura_canvas = 280.0f;

        if (largura_total >= 760.0f) {
            float largura_esquema_viga = largura_total * 0.66f;
            float largura_secao = largura_total - largura_esquema_viga - espacamento;

            if (largura_esquema_viga < 360.0f) {
                largura_esquema_viga = 360.0f;
            }

            if (largura_secao < 230.0f) {
                largura_secao = 230.0f;
            }

            ImGui::BeginGroup();
            ImGui::Text("Esquema estrutural");
            desenharEsquemaVigaBiapoiada(viga, largura_esquema_viga, altura_canvas);
            ImGui::EndGroup();

            ImGui::SameLine(0.0f, espacamento);

            ImGui::BeginGroup();
            ImGui::Text("Seção transversal ");
            ImGui::Text("(cabo no meio da seção)");
            desenharSecaoRetangular(viga, largura_secao, altura_canvas);
            ImGui::EndGroup();
        }
        else {
            ImGui::Text("Seção transversal ");
            ImGui::Text("(cabo no meio da seção)");
            desenharEsquemaVigaBiapoiada(viga, largura_total, altura_canvas);

            ImGui::Spacing();
            ImGui::Text("Seção transversal");
            desenharSecaoRetangular(viga, largura_total, altura_canvas);
        }

        ImGui::Spacing();
        ImGui::Text("Traçado completo do cabo");
        desenharTracadoCompletoCabo(viga, largura_total, 230.0f);
    }


    std::vector<VigaConcretoProtendido::Cabo::Ponto> criarPontosTensaoAtrito(VigaConcretoProtendido& viga){
        std::vector<VigaConcretoProtendido::Cabo::Ponto> pontos_grafico;

        for (int i = 0; i < (int)viga.cabo.pontos.size(); i++){
            VigaConcretoProtendido::Cabo::Ponto ponto;
            ponto.x = viga.cabo.pontos[i].x;
            ponto.y = viga.cabo.pontos[i].tensao_apos_atrito_MPa;
            pontos_grafico.push_back(ponto);
        }

        return pontos_grafico;
    }

    std::vector<VigaConcretoProtendido::Cabo::Ponto> criarPontosEncunhamento(VigaConcretoProtendido& viga){
        std::vector<VigaConcretoProtendido::Cabo::Ponto> pontos_grafico;

        for (int i = 0; i < (int)viga.cabo.pontos.size(); i++){
            VigaConcretoProtendido::Cabo::Ponto ponto;
            ponto.x = viga.cabo.pontos[i].x;
            ponto.y = viga.cabo.pontos[i].tensao_apos_encunhamento_MPa;
            pontos_grafico.push_back(ponto);
        }

        return pontos_grafico;
    }

    std::vector<VigaConcretoProtendido::Cabo::Ponto> criarPontosTensaoAposEncurtamento(VigaConcretoProtendido& viga){
        std::vector<VigaConcretoProtendido::Cabo::Ponto> pontos_grafico;

        for (int i = 0; i < (int)viga.cabo.pontos.size(); i++){
            VigaConcretoProtendido::Cabo::Ponto ponto;
            ponto.x = viga.cabo.pontos[i].x;
            ponto.y = viga.cabo.pontos[i].tensao_apos_encurtamento_MPa;
            pontos_grafico.push_back(ponto);
        }

        return pontos_grafico;
    }

    std::vector<VigaConcretoProtendido::Cabo::Ponto> criarRetanguloViga(double vao, double altura_viga, double y_topo){
        std::vector<VigaConcretoProtendido::Cabo::Ponto> viga;
        double y_base = y_topo - altura_viga;

        VigaConcretoProtendido::Cabo::Ponto p1;
        p1.x = 0.0;
        p1.y = y_topo;

        VigaConcretoProtendido::Cabo::Ponto p2;
        p2.x = vao;
        p2.y = y_topo;

        VigaConcretoProtendido::Cabo::Ponto p3;
        p3.x = vao;
        p3.y = y_base;

        VigaConcretoProtendido::Cabo::Ponto p4;
        p4.x = 0.0;
        p4.y = y_base;

        VigaConcretoProtendido::Cabo::Ponto p5;
        p5.x = 0.0;
        p5.y = y_topo;

        viga.push_back(p1);
        viga.push_back(p2);
        viga.push_back(p3);
        viga.push_back(p4);
        viga.push_back(p5);

        return viga;
    }


    void plotarPontos(
        std::vector<VigaConcretoProtendido::Cabo::Ponto>& pontos,
        std::string titulo,
        std::string nome_linha,
        std::string eixo_x,
        std::string eixo_y,
        float altura,
        std::vector<VigaConcretoProtendido::Cabo::Ponto>* pontos_extra,
        std::string nome_linha_extra
    )
    {
        std::vector<double> x;
        std::vector<double> y;

        for (int i = 0; i < (int)pontos.size(); i++) {
            x.push_back(pontos[i].x);
            y.push_back(pontos[i].y);
        }

        std::vector<double> x_extra;
        std::vector<double> y_extra;

        double ymin = y.empty() ? 0.0 : y[0];
        double ymax = y.empty() ? 1.0 : y[0];

        for (double valor : y) {
            ymin = menorValor(ymin, valor);
            ymax = maiorValor(ymax, valor);
        }

        if (pontos_extra != 0) {
            for (int i = 0; i < (int)pontos_extra->size(); i++) {
                x_extra.push_back((*pontos_extra)[i].x);
                y_extra.push_back((*pontos_extra)[i].y);
                ymin = menorValor(ymin, (*pontos_extra)[i].y);
                ymax = maiorValor(ymax, (*pontos_extra)[i].y);
            }
        }

        ImVec2 tamanho_plot(ImGui::GetContentRegionAvail().x, altura);
        double vao = x.empty() ? 1.0 : x.back();

        if (ImPlot::BeginPlot(titulo.c_str(), tamanho_plot)) {
            configurarEixosGrafico(
                vao,
                eixo_x.c_str(),
                eixo_y.c_str(),
                ymin,
                ymax
            );

            ImPlot::PlotLine(nome_linha.c_str(), x.data(), y.data(), (int)x.size());

            if (pontos_extra != 0) {
                ImPlot::PlotLine(
                    nome_linha_extra.c_str(),
                    x_extra.data(),
                    y_extra.data(),
                    (int)x_extra.size()
                );
            }

            ImPlot::EndPlot();
        }
    }

    void plotarPerdasProgressivas(VigaConcretoProtendido& viga, float altura)
    {
        std::vector<double> x;
        std::vector<double> perdas;
        std::vector<double> eixo_zero;

        double ymax = 1.0;

        for (int i = 0; i < (int)viga.cabo.pontos.size(); i++) {
            x.push_back(viga.cabo.pontos[i].x);
            perdas.push_back(viga.cabo.pontos[i].perda_progressiva_total_MPa);
            eixo_zero.push_back(0.0);
            ymax = maiorValor(ymax, perdas.back());
        }

        if (ImPlot::BeginPlot(
            "Perdas progressivas",
            ImVec2(ImGui::GetContentRegionAvail().x, altura)
        )) {
            configurarEixosGrafico(
                viga.vao,
                "x (m)",
                "Perda de tensão (MPa)",
                0.0,
                ymax,
                false,
                true
            );

            ImPlotSpec preenchimento_perdas;
            preenchimento_perdas.FillColor = ImVec4(0.82f, 0.42f, 0.18f, 1.0f);
            preenchimento_perdas.FillAlpha = 0.35f;

            ImPlot::PlotShaded(
                "Área das perdas progressivas",
                x.data(),
                eixo_zero.data(),
                perdas.data(),
                (int)x.size(),
                preenchimento_perdas
            );

            ImPlot::PlotLine(
                "Δσp progressiva",
                x.data(),
                perdas.data(),
                (int)x.size()
            );

            ImPlot::EndPlot();
        }
    }

    void plotarAtoProtensao(VigaConcretoProtendido& viga, float altura)
    {
        std::vector<double> x;
        std::vector<double> limite_tracao;
        std::vector<double> limite_compressao;
        std::vector<double> tensao_topo;
        std::vector<double> tensao_base;

        double ymin = -viga.limite_compressao_ato_MPa;
        double ymax = viga.limite_tracao_ato_MPa;

        for (int i = 0; i < (int)viga.cabo.pontos.size(); i++) {
            x.push_back(viga.cabo.pontos[i].x);
            limite_tracao.push_back(viga.cabo.pontos[i].limite_tracao_ato_MPa);
            limite_compressao.push_back(-viga.cabo.pontos[i].limite_compressao_ato_MPa);
            tensao_topo.push_back(viga.cabo.pontos[i].tensao_topo_ato_MPa);
            tensao_base.push_back(viga.cabo.pontos[i].tensao_base_ato_MPa);

            ymin = menorValor(ymin, tensao_topo.back());
            ymin = menorValor(ymin, tensao_base.back());
            ymax = maiorValor(ymax, tensao_topo.back());
            ymax = maiorValor(ymax, tensao_base.back());
        }

        if (ImPlot::BeginPlot(
            "Ato da protensão",
            ImVec2(ImGui::GetContentRegionAvail().x, altura)
        )) {
            configurarEixosGrafico(
                viga.vao,
                "x (m)",
                "Tensão (MPa)",
                ymin,
                ymax
            );

            ImPlotSpec faixa_permitida;
            faixa_permitida.FillColor = ImVec4(0.20f, 0.65f, 0.30f, 1.0f);
            faixa_permitida.FillAlpha = 0.38f;

            ImPlotSpec faixa_tensoes;
            faixa_tensoes.FillColor = ImVec4(0.20f, 0.35f, 0.85f, 1.0f);
            faixa_tensoes.FillAlpha = 0.45f;

            ImPlot::PlotShaded(
                "Faixa permitida",
                x.data(),
                limite_compressao.data(),
                limite_tracao.data(),
                (int)x.size(),
                faixa_permitida
            );

            ImPlot::PlotShaded(
                "Faixa topo-base",
                x.data(),
                tensao_base.data(),
                tensao_topo.data(),
                (int)x.size(),
                faixa_tensoes
            );

            ImPlot::PlotLine("Limite de tração", x.data(), limite_tracao.data(), (int)x.size());
            ImPlot::PlotLine("Limite de compressão", x.data(), limite_compressao.data(), (int)x.size());
            ImPlot::PlotLine("Fibra superior", x.data(), tensao_topo.data(), (int)x.size());
            ImPlot::PlotLine("Fibra inferior", x.data(), tensao_base.data(), (int)x.size());

            ImPlot::EndPlot();
        }
    }

    void plotarELSF(VigaConcretoProtendido& viga, float altura)
    {
        std::vector<double> x;
        std::vector<double> limite_tracao;
        std::vector<double> limite_compressao;
        std::vector<double> tensao_topo;
        std::vector<double> tensao_base;

        double ymin = -viga.limite_compressao_els_f_MPa;
        double ymax = viga.limite_tracao_fctf_MPa;

        for (int i = 0; i < (int)viga.cabo.pontos.size(); i++) {
            x.push_back(viga.cabo.pontos[i].x);
            limite_tracao.push_back(viga.cabo.pontos[i].limite_tracao_fctf_MPa);
            limite_compressao.push_back(-viga.cabo.pontos[i].limite_compressao_els_f_MPa);
            tensao_topo.push_back(viga.cabo.pontos[i].tensao_topo_els_f_MPa);
            tensao_base.push_back(viga.cabo.pontos[i].tensao_base_els_f_MPa);

            ymin = menorValor(ymin, tensao_topo.back());
            ymin = menorValor(ymin, tensao_base.back());
            ymax = maiorValor(ymax, tensao_topo.back());
            ymax = maiorValor(ymax, tensao_base.back());
        }

        if (ImPlot::BeginPlot(
            "Estado Limite de Formação de Fissuras",
            ImVec2(ImGui::GetContentRegionAvail().x, altura)
        )) {
            configurarEixosGrafico(
                viga.vao,
                "x (m)",
                "Tensão (MPa)",
                ymin,
                ymax
            );

            ImPlotSpec faixa_permitida;
            faixa_permitida.FillColor = ImVec4(0.20f, 0.65f, 0.30f, 1.0f);
            faixa_permitida.FillAlpha = 0.38f;

            ImPlotSpec faixa_tensoes;
            faixa_tensoes.FillColor = ImVec4(0.20f, 0.35f, 0.85f, 1.0f);
            faixa_tensoes.FillAlpha = 0.45f;

            ImPlot::PlotShaded(
                "Faixa permitida",
                x.data(),
                limite_compressao.data(),
                limite_tracao.data(),
                (int)x.size(),
                faixa_permitida
            );

            ImPlot::PlotShaded(
                "Faixa topo-base",
                x.data(),
                tensao_base.data(),
                tensao_topo.data(),
                (int)x.size(),
                faixa_tensoes
            );

            ImPlot::PlotLine("Limite de tração", x.data(), limite_tracao.data(), (int)x.size());
            ImPlot::PlotLine("Limite de compressão", x.data(), limite_compressao.data(), (int)x.size());
            ImPlot::PlotLine("Fibra superior", x.data(), tensao_topo.data(), (int)x.size());
            ImPlot::PlotLine("Fibra inferior", x.data(), tensao_base.data(), (int)x.size());

            ImPlot::EndPlot();
        }
    }

    void plotarELSD(VigaConcretoProtendido& viga, float altura)
    {
        std::vector<double> x;
        std::vector<double> limite_tracao;
        std::vector<double> limite_compressao;
        std::vector<double> tensao_topo;
        std::vector<double> tensao_base;

        double ymin = -viga.limite_compressao_els_d_MPa;
        double ymax = viga.limite_tracao_els_d_MPa;

        for (int i = 0; i < (int)viga.cabo.pontos.size(); i++) {
            x.push_back(viga.cabo.pontos[i].x);
            limite_tracao.push_back(viga.cabo.pontos[i].limite_tracao_els_d_MPa);
            limite_compressao.push_back(-viga.cabo.pontos[i].limite_compressao_els_d_MPa);
            tensao_topo.push_back(viga.cabo.pontos[i].tensao_topo_els_d_MPa);
            tensao_base.push_back(viga.cabo.pontos[i].tensao_base_els_d_MPa);

            ymin = menorValor(ymin, tensao_topo.back());
            ymin = menorValor(ymin, tensao_base.back());
            ymax = maiorValor(ymax, tensao_topo.back());
            ymax = maiorValor(ymax, tensao_base.back());
        }

        if (ImPlot::BeginPlot(
            "Estado Limite de Descompressão",
            ImVec2(ImGui::GetContentRegionAvail().x, altura)
        )) {
            configurarEixosGrafico(
                viga.vao,
                "x (m)",
                "Tensão (MPa)",
                ymin,
                ymax
            );

            ImPlotSpec faixa_permitida;
            faixa_permitida.FillColor = ImVec4(0.20f, 0.65f, 0.30f, 1.0f);
            faixa_permitida.FillAlpha = 0.38f;

            ImPlotSpec faixa_tensoes;
            faixa_tensoes.FillColor = ImVec4(0.20f, 0.35f, 0.85f, 1.0f);
            faixa_tensoes.FillAlpha = 0.45f;

            ImPlot::PlotShaded(
                "Faixa permitida",
                x.data(),
                limite_compressao.data(),
                limite_tracao.data(),
                (int)x.size(),
                faixa_permitida
            );

            ImPlot::PlotShaded(
                "Faixa topo-base",
                x.data(),
                tensao_base.data(),
                tensao_topo.data(),
                (int)x.size(),
                faixa_tensoes
            );

            ImPlot::PlotLine("Limite de tração", x.data(), limite_tracao.data(), (int)x.size());
            ImPlot::PlotLine("Limite de compressão", x.data(), limite_compressao.data(), (int)x.size());
            ImPlot::PlotLine("Fibra superior", x.data(), tensao_topo.data(), (int)x.size());
            ImPlot::PlotLine("Fibra inferior", x.data(), tensao_base.data(), (int)x.size());

            ImPlot::EndPlot();
        }
    }

    void aplicarEscalaVisualGraficoELU()
    {
        // Aumenta apenas a escala visual dos textos dos gráficos finais
        // para melhorar a leitura em capturas de tela.
        ImGui::SetWindowFontScale(1.18f);
    }

    void restaurarEscalaVisualGraficoELU()
    {
        ImGui::SetWindowFontScale(1.00f);
    }

    void plotarELU(VigaConcretoProtendido& viga, float altura)
    {
        std::vector<double> x;
        std::vector<double> msd;
        std::vector<double> eixo_zero;

        double ymax = 1.0;

        for (int i = 0; i < (int)viga.cabo.pontos.size(); i++) {
            x.push_back(viga.cabo.pontos[i].x);
            msd.push_back(viga.cabo.pontos[i].momento_solicitante_elu_kNm);
            eixo_zero.push_back(0.0);
            ymax = maiorValor(ymax, msd.back());
        }

        aplicarEscalaVisualGraficoELU();

        if (ImPlot::BeginPlot(
            "ELU - Momento solicitante",
            ImVec2(ImGui::GetContentRegionAvail().x, altura)
        )) {
            configurarEixosGrafico(
                viga.vao,
                "x (m)",
                "Momento solicitante (kN.m)",
                0.0,
                ymax,
                true,
                true
            );

            ImPlotSpec preenchimento_momento;
            preenchimento_momento.FillColor = ImVec4(0.20f, 0.35f, 0.85f, 1.0f);
            preenchimento_momento.FillAlpha = 0.45f;

            ImPlot::PlotShaded(
                "Área do diagrama de momento",
                x.data(),
                eixo_zero.data(),
                msd.data(),
                (int)x.size(),
                preenchimento_momento
            );

            ImPlot::PlotLine("Msd em ELU", x.data(), msd.data(), (int)x.size());

            ImPlot::EndPlot();
        }

        restaurarEscalaVisualGraficoELU();
    }

    void plotarAsELU(VigaConcretoProtendido& viga, float altura)
    {
        std::vector<double> x;
        std::vector<double> as;
        std::vector<double> as_min;
        std::vector<double> eixo_zero;

        double valor_as_min = CalcularAsMinimaGraficaCm2(viga);
        double ymax = maiorValor(1.0, valor_as_min);

        for (int i = 0; i < (int)viga.cabo.pontos.size(); i++) {
            x.push_back(viga.cabo.pontos[i].x);
            as.push_back(viga.cabo.pontos[i].As_elu * 1.0e4);
            as_min.push_back(valor_as_min);
            eixo_zero.push_back(0.0);
            ymax = maiorValor(ymax, as.back());
        }

        aplicarEscalaVisualGraficoELU();

        if (ImPlot::BeginPlot(
            "ELU - Armadura passiva necessária",
            ImVec2(ImGui::GetContentRegionAvail().x, altura)
        )) {
            configurarEixosGrafico(
                viga.vao,
                "x (m)",
                "As (cm2)",
                0.0,
                ymax,
                false,
                true
            );

            ImPlotSpec preenchimento_as;
            preenchimento_as.FillColor = ImVec4(0.20f, 0.35f, 0.85f, 1.0f);
            preenchimento_as.FillAlpha = 0.45f;

            ImPlot::PlotShaded(
                "Área de armadura necessária",
                x.data(),
                eixo_zero.data(),
                as.data(),
                (int)x.size(),
                preenchimento_as
            );

            ImPlot::PlotLine("As,nec", x.data(), as.data(), (int)x.size());
            ImPlot::PlotLine("As,min = 0,15% Ac", x.data(), as_min.data(), (int)x.size());

            ImPlot::EndPlot();
        }

        restaurarEscalaVisualGraficoELU();
    }

    void desenharStatusTabela(bool aprovado)
    {
        ImVec4 cor = aprovado
            ? ImVec4(0.10f, 0.60f, 0.20f, 1.0f)
            : ImVec4(0.80f, 0.15f, 0.10f, 1.0f);

        ImGui::TextColored(cor, aprovado ? "OK" : "ERRO");
    }

    void desenharTabelaAtrito(VigaConcretoProtendido& viga)
    {
        ImGuiTableFlags flags =
            ImGuiTableFlags_Borders |
            ImGuiTableFlags_RowBg |
            ImGuiTableFlags_ScrollX |
            ImGuiTableFlags_ScrollY |
            ImGuiTableFlags_SizingFixedFit;

        if (ImGui::BeginTable(
            "TabelaAtrito",
            10,
            flags,
            ImVec2(0.0f, 330.0f)
        )) {
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableSetupColumn("Trecho");
            ImGui::TableSetupColumn("x (m)");
            ImGui::TableSetupColumn("h(i-1) (cm)");
            ImGui::TableSetupColumn("h(i) (cm)");
            ImGui::TableSetupColumn("Δh(i) (cm)");
            ImGui::TableSetupColumn("β (rad)");
            ImGui::TableSetupColumn("α (rad)");
            ImGui::TableSetupColumn("Σα (rad)");
            ImGui::TableSetupColumn("ΔP atr (kN)");
            ImGui::TableSetupColumn("σp após atrito (MPa)");
            ImGui::TableHeadersRow();

            for (int i = 0; i < (int)viga.cabo.pontos.size(); i++) {
                double h_inicial_cm = viga.cabo.pontos[i].y * 100.0;
                double h_final_cm = h_inicial_cm;
                double delta_h_cm = 0.0;
                double beta = 0.0;
                double alfa = 0.0;
                double alfa_acumulado = 0.0;
                double perda_forca_atrito_kN = 0.0;

                if (i > 0 && i - 1 < (int)viga.cabo.trechos.size()) {
                    const auto& trecho = viga.cabo.trechos[i - 1];
                    h_inicial_cm = trecho.altura_inicial * 100.0;
                    h_final_cm = trecho.altura_final * 100.0;
                    delta_h_cm = trecho.dy * 100.0;
                    beta = trecho.beta;
                    alfa = trecho.alfa;
                    alfa_acumulado = trecho.alfa_acumulado;
                    perda_forca_atrito_kN = trecho.perda_forca_atrito / 1000.0;
                }

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted(nomeTrecho(i).c_str());
                ImGui::TableSetColumnIndex(1); ImGui::Text("%.2f", viga.cabo.pontos[i].x);
                ImGui::TableSetColumnIndex(2); ImGui::Text("%.2f", h_inicial_cm);
                ImGui::TableSetColumnIndex(3); ImGui::Text("%.2f", h_final_cm);
                ImGui::TableSetColumnIndex(4); ImGui::Text("%.2f", delta_h_cm);
                ImGui::TableSetColumnIndex(5); ImGui::Text("%.4f", beta);
                ImGui::TableSetColumnIndex(6); ImGui::Text("%.4f", alfa);
                ImGui::TableSetColumnIndex(7); ImGui::Text("%.4f", alfa_acumulado);
                ImGui::TableSetColumnIndex(8); ImGui::Text("%.2f", perda_forca_atrito_kN);
                ImGui::TableSetColumnIndex(9); ImGui::Text("%.2f", viga.cabo.pontos[i].tensao_apos_atrito_MPa);
            }

            ImGui::EndTable();
        }
    }

    void desenharTabelaEncunhamento(VigaConcretoProtendido& viga)
    {
        ImGuiTableFlags flags =
            ImGuiTableFlags_Borders |
            ImGuiTableFlags_RowBg |
            ImGuiTableFlags_ScrollX |
            ImGuiTableFlags_ScrollY |
            ImGuiTableFlags_SizingFixedFit;

        if (ImGui::BeginTable(
            "TabelaEncunhamento",
            7,
            flags,
            ImVec2(0.0f, 330.0f)
        )) {
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableSetupColumn("Ponto");
            ImGui::TableSetupColumn("Trecho");
            ImGui::TableSetupColumn("x (m)");
            ImGui::TableSetupColumn("ep (m)");
            ImGui::TableSetupColumn("σp antes do encunhamento (MPa)");
            ImGui::TableSetupColumn("Δσp,enc (MPa)");
            ImGui::TableSetupColumn("σp após encunhamento (MPa)");
            ImGui::TableHeadersRow();

            for (int i = 0; i < (int)viga.cabo.pontos.size(); i++) {
                const auto& ponto = viga.cabo.pontos[i];
                double perda_encunhamento =
                    ponto.tensao_apos_atrito_MPa
                    - ponto.tensao_apos_encunhamento_MPa;

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("%d", i);
                ImGui::TableSetColumnIndex(1); ImGui::TextUnformatted(nomeTrecho(i).c_str());
                ImGui::TableSetColumnIndex(2); ImGui::Text("%.2f", ponto.x);
                ImGui::TableSetColumnIndex(3); ImGui::Text("%.4f", ponto.excentricidade);
                ImGui::TableSetColumnIndex(4); ImGui::Text("%.2f", ponto.tensao_apos_atrito_MPa);
                ImGui::TableSetColumnIndex(5); ImGui::Text("%.2f", perda_encunhamento);
                ImGui::TableSetColumnIndex(6); ImGui::Text("%.2f", ponto.tensao_apos_encunhamento_MPa);
            }

            ImGui::EndTable();
        }
    }

    void desenharTabelaEncurtamento(VigaConcretoProtendido& viga)
    {
        ImGuiTableFlags flags =
            ImGuiTableFlags_Borders |
            ImGuiTableFlags_RowBg |
            ImGuiTableFlags_ScrollX |
            ImGuiTableFlags_ScrollY |
            ImGuiTableFlags_SizingFixedFit;

        if (ImGui::BeginTable(
            "TabelaEncurtamento",
            8,
            flags,
            ImVec2(0.0f, 330.0f)
        )) {
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableSetupColumn("Trecho");
            ImGui::TableSetupColumn("x (m)");
            ImGui::TableSetupColumn("Mpp (kN.m)");
            ImGui::TableSetupColumn("ep (m)");
            ImGui::TableSetupColumn("P0 (kN)");
            ImGui::TableSetupColumn("σcp + σcg (MPa)");
            ImGui::TableSetupColumn("Δσp,enc (MPa)");
            ImGui::TableSetupColumn("σp após encurtamento elástico (MPa)");
            ImGui::TableHeadersRow();

            for (int i = 0; i < (int)viga.cabo.pontos.size(); i++) {
                const auto& ponto = viga.cabo.pontos[i];

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted(nomeTrecho(i).c_str());
                ImGui::TableSetColumnIndex(1); ImGui::Text("%.2f", ponto.x);
                ImGui::TableSetColumnIndex(2); ImGui::Text("%.2f", ponto.momento_peso_proprio_kNm);
                ImGui::TableSetColumnIndex(3); ImGui::Text("%.4f", ponto.excentricidade);
                ImGui::TableSetColumnIndex(4); ImGui::Text("%.2f", ponto.forca_protensao_kN);
                ImGui::TableSetColumnIndex(5); ImGui::Text("%.3f", ponto.tensao_concreto_no_nivel_do_cabo_MPa);
                ImGui::TableSetColumnIndex(6); ImGui::Text("%.3f", ponto.perda_encurtamento_MPa);
                ImGui::TableSetColumnIndex(7); ImGui::Text("%.2f", ponto.tensao_apos_encurtamento_MPa);
            }

            ImGui::EndTable();
        }
    }

    void desenharTabelaAto(VigaConcretoProtendido& viga)
    {
        ImGuiTableFlags flags =
            ImGuiTableFlags_Borders |
            ImGuiTableFlags_RowBg |
            ImGuiTableFlags_ScrollX |
            ImGuiTableFlags_ScrollY |
            ImGuiTableFlags_SizingFixedFit;

        if (ImGui::BeginTable(
            "TabelaAto",
            7,
            flags,
            ImVec2(0.0f, 390.0f)
        )) {
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableSetupColumn("Trecho");
            ImGui::TableSetupColumn("x (m)");
            ImGui::TableSetupColumn("Pi · γp (kN)");
            ImGui::TableSetupColumn("ep (m)");
            ImGui::TableSetupColumn("Mpp (kN.m)");
            ImGui::TableSetupColumn("σi (topo) (MPa)");
            ImGui::TableSetupColumn("σi (base) (MPa)");
            ImGui::TableHeadersRow();

            for (int i = 0; i < (int)viga.cabo.pontos.size(); i++) {
                const auto& ponto = viga.cabo.pontos[i];

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted(nomeTrecho(i).c_str());
                ImGui::TableSetColumnIndex(1); ImGui::Text("%.2f", ponto.x);
                ImGui::TableSetColumnIndex(2); ImGui::Text("%.2f", ponto.forca_protensao_ato_kN);
                ImGui::TableSetColumnIndex(3); ImGui::Text("%.4f", ponto.excentricidade);
                ImGui::TableSetColumnIndex(4); ImGui::Text("%.2f", ponto.momento_peso_proprio_ato_kNm);
                ImGui::TableSetColumnIndex(5); ImGui::Text("%.3f", ponto.tensao_topo_ato_MPa);
                ImGui::TableSetColumnIndex(6); ImGui::Text("%.3f", ponto.tensao_base_ato_MPa);
            }

            ImGui::EndTable();
        }
    }

    void desenharTabelaPerdasProgressivas(VigaConcretoProtendido& viga)
    {
        ImGuiTableFlags flags =
            ImGuiTableFlags_Borders |
            ImGuiTableFlags_RowBg |
            ImGuiTableFlags_ScrollX |
            ImGuiTableFlags_ScrollY |
            ImGuiTableFlags_SizingFixedFit;

        if (ImGui::BeginTable(
            "TabelaPerdasProgressivas",
            10,
            flags,
            ImVec2(0.0f, 390.0f)
        )) {
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableSetupColumn("Trecho");
            ImGui::TableSetupColumn("x (m)");
            ImGui::TableSetupColumn("ep (m)");
            ImGui::TableSetupColumn("η");
            ImGui::TableSetupColumn("εcs∞");
            ImGui::TableSetupColumn("εcs∞(t0)");
            ImGui::TableSetupColumn("φ total");
            ImGui::TableSetupColumn("χ∞ (%)");
            ImGui::TableSetupColumn("Δσp prog (MPa)");
            ImGui::TableSetupColumn("σp após perdas prog (MPa)");
            ImGui::TableHeadersRow();

            for (int i = 0; i < (int)viga.cabo.pontos.size(); i++) {
                const auto& ponto = viga.cabo.pontos[i];
                const auto& perdas = viga.perdas_progressivas;

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted(nomeTrecho(i).c_str());
                ImGui::TableSetColumnIndex(1); ImGui::Text("%.2f", ponto.x);
                ImGui::TableSetColumnIndex(2); ImGui::Text("%.4f", ponto.excentricidade);
                ImGui::TableSetColumnIndex(3); ImGui::Text("%.3f", ponto.eta);
                ImGui::TableSetColumnIndex(4); ImGui::Text("%.6f", perdas.retracao_concreto_infinito);
                ImGui::TableSetColumnIndex(5); ImGui::Text("%.6f", perdas.retracao_concreto_infinito_t0);
                ImGui::TableSetColumnIndex(6); ImGui::Text("%.3f", perdas.phi_total);
                ImGui::TableSetColumnIndex(7); ImGui::Text("%.3f", perdas.coeficiente_fluencia_aco_qui_infinito_percento);
                ImGui::TableSetColumnIndex(8); ImGui::Text("%.3f", ponto.perda_progressiva_total_MPa);
                ImGui::TableSetColumnIndex(9); ImGui::Text("%.3f", ponto.tensao_apos_perda_progressiva__MPa);
            }

            ImGui::EndTable();
        }
    }

    void desenharTabelaELSF(VigaConcretoProtendido& viga)
    {
        ImGuiTableFlags flags =
            ImGuiTableFlags_Borders |
            ImGuiTableFlags_RowBg |
            ImGuiTableFlags_ScrollX |
            ImGuiTableFlags_ScrollY |
            ImGuiTableFlags_SizingFixedFit;

        if (ImGui::BeginTable(
            "TabelaELSF",
            9,
            flags,
            ImVec2(0.0f, 390.0f)
        )) {
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableSetupColumn("Trecho");
            ImGui::TableSetupColumn("x (m)");
            ImGui::TableSetupColumn("P∞ (kN)");
            ImGui::TableSetupColumn("ep (m)");
            ImGui::TableSetupColumn("Mpp (kN.m)");
            ImGui::TableSetupColumn("Mq (kN.m)");
            ImGui::TableSetupColumn("MCF (kN.m)");
            ImGui::TableSetupColumn("σCF (topo) (MPa)");
            ImGui::TableSetupColumn("σCF (base) (MPa)");
            ImGui::TableHeadersRow();

            for (int i = 0; i < (int)viga.cabo.pontos.size(); i++) {
                const auto& ponto = viga.cabo.pontos[i];

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted(nomeTrecho(i).c_str());
                ImGui::TableSetColumnIndex(1); ImGui::Text("%.2f", ponto.x);
                ImGui::TableSetColumnIndex(2); ImGui::Text("%.2f", ponto.forca_protensao_els_f_kN);
                ImGui::TableSetColumnIndex(3); ImGui::Text("%.4f", ponto.excentricidade);
                ImGui::TableSetColumnIndex(4); ImGui::Text("%.2f", ponto.momento_peso_proprio_kNm);
                ImGui::TableSetColumnIndex(5); ImGui::Text("%.2f", ponto.momento_devido_carga_kNm);
                ImGui::TableSetColumnIndex(6); ImGui::Text("%.2f", ponto.momento_els_f_kNm);
                ImGui::TableSetColumnIndex(7); ImGui::Text("%.3f", ponto.tensao_topo_els_f_MPa);
                ImGui::TableSetColumnIndex(8); ImGui::Text("%.3f", ponto.tensao_base_els_f_MPa);
            }

            ImGui::EndTable();
        }
    }

    void desenharTabelaELSD(VigaConcretoProtendido& viga)
    {
        ImGuiTableFlags flags =
            ImGuiTableFlags_Borders |
            ImGuiTableFlags_RowBg |
            ImGuiTableFlags_ScrollX |
            ImGuiTableFlags_ScrollY |
            ImGuiTableFlags_SizingFixedFit;

        if (ImGui::BeginTable(
            "TabelaELSD",
            9,
            flags,
            ImVec2(0.0f, 390.0f)
        )) {
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableSetupColumn("Trecho");
            ImGui::TableSetupColumn("x (m)");
            ImGui::TableSetupColumn("P∞ (kN)");
            ImGui::TableSetupColumn("ep (m)");
            ImGui::TableSetupColumn("Mpp (kN.m)");
            ImGui::TableSetupColumn("Mq (kN.m)");
            ImGui::TableSetupColumn("MCQP (kN.m)");
            ImGui::TableSetupColumn("σCQP (topo) (MPa)");
            ImGui::TableSetupColumn("σCQP (base) (MPa)");
            ImGui::TableHeadersRow();

            for (int i = 0; i < (int)viga.cabo.pontos.size(); i++) {
                const auto& ponto = viga.cabo.pontos[i];

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted(nomeTrecho(i).c_str());
                ImGui::TableSetColumnIndex(1); ImGui::Text("%.2f", ponto.x);
                ImGui::TableSetColumnIndex(2); ImGui::Text("%.2f", ponto.forca_protensao_els_d_kN);
                ImGui::TableSetColumnIndex(3); ImGui::Text("%.4f", ponto.excentricidade);
                ImGui::TableSetColumnIndex(4); ImGui::Text("%.2f", ponto.momento_peso_proprio_kNm);
                ImGui::TableSetColumnIndex(5); ImGui::Text("%.2f", ponto.momento_devido_carga_kNm);
                ImGui::TableSetColumnIndex(6); ImGui::Text("%.2f", ponto.momento_els_d_kNm);
                ImGui::TableSetColumnIndex(7); ImGui::Text("%.3f", ponto.tensao_topo_els_d_MPa);
                ImGui::TableSetColumnIndex(8); ImGui::Text("%.3f", ponto.tensao_base_els_d_MPa);
            }

            ImGui::EndTable();
        }
    }

    void desenharTabelaELU(VigaConcretoProtendido& viga)
    {
        ImGuiTableFlags flags =
            ImGuiTableFlags_Borders |
            ImGuiTableFlags_RowBg |
            ImGuiTableFlags_ScrollX |
            ImGuiTableFlags_ScrollY |
            ImGuiTableFlags_SizingFixedFit;

        double as_min = CalcularAsMinimaGraficaCm2(viga);

        if (ImGui::BeginTable(
            "TabelaELU",
            14,
            flags,
            ImVec2(0.0f, 430.0f)
        )) {
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableSetupColumn("Ponto");
            ImGui::TableSetupColumn("Trecho");
            ImGui::TableSetupColumn("x (m)");
            ImGui::TableSetupColumn("e (m)");
            ImGui::TableSetupColumn("P efetiva (kN)");
            ImGui::TableSetupColumn("P ELU (kN)");
            ImGui::TableSetupColumn("Δσp (MPa)");
            ImGui::TableSetupColumn("Mpp (kN.m)");
            ImGui::TableSetupColumn("Mq (kN.m)");
            ImGui::TableSetupColumn("Msd (kN.m)");
            ImGui::TableSetupColumn("x/d");
            ImGui::TableSetupColumn("As,nec (cm2)");
            ImGui::TableSetupColumn("As,min (cm2)");
            ImGui::TableSetupColumn("Status");
            ImGui::TableHeadersRow();

            for (int i = 0; i < (int)viga.cabo.pontos.size(); i++) {
                const auto& ponto = viga.cabo.pontos[i];
                double as_nec = ponto.As_elu * 1.0e4;

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("%d", i);
                ImGui::TableSetColumnIndex(1); ImGui::TextUnformatted(nomeTrecho(i).c_str());
                ImGui::TableSetColumnIndex(2); ImGui::Text("%.2f", ponto.x);
                ImGui::TableSetColumnIndex(3); ImGui::Text("%.4f", ponto.excentricidade);
                ImGui::TableSetColumnIndex(4); ImGui::Text("%.2f", ponto.forca_apos_perda_progressiva_kN);
                ImGui::TableSetColumnIndex(5); ImGui::Text("%.2f", ponto.forca_protensao_elu_kN);
                ImGui::TableSetColumnIndex(6); ImGui::Text("%.2f", ponto.delta_sigma_p_elu_MPa);
                ImGui::TableSetColumnIndex(7); ImGui::Text("%.2f", ponto.momento_peso_proprio_elu_kNm);
                ImGui::TableSetColumnIndex(8); ImGui::Text("%.2f", ponto.momento_carga_elu_kNm);
                ImGui::TableSetColumnIndex(9); ImGui::Text("%.2f", ponto.momento_solicitante_elu_kNm);
                ImGui::TableSetColumnIndex(10); ImGui::Text("%.4f", ponto.x_sobre_d_elu);
                ImGui::TableSetColumnIndex(11); ImGui::Text("%.2f", as_nec);
                ImGui::TableSetColumnIndex(12); ImGui::Text("%.2f", as_min);
                ImGui::TableSetColumnIndex(13); desenharStatusTabela(ponto.elu_aprovado_no_ponto);
            }

            ImGui::EndTable();
        }
    }

    void desenharPagina(VigaConcretoProtendido& viga, int pagina)
    {
        std::vector<VigaConcretoProtendido::Cabo::Ponto> curva_atrito =
            criarPontosTensaoAtrito(viga);

        std::vector<VigaConcretoProtendido::Cabo::Ponto> curva_encunhamento =
            criarPontosEncunhamento(viga);

        std::vector<VigaConcretoProtendido::Cabo::Ponto> curva_encurtamento =
            criarPontosTensaoAposEncurtamento(viga);

        if (pagina == 2) {
            ImGui::Text("Perdas imediatas por atrito e encunhamento");
            ImGui::Text(
                "Encunhamento: %s | iterações: %d | perda na ancoragem: %.3f MPa",
                viga.cabo.encunhamento_convergiu ? "convergente" : "não convergente",
                viga.cabo.iteracoes_encunhamento,
                viga.cabo.perda_na_ancoragem_encunhamento_MPa
            );

            ImGui::Spacing();
            plotarPontos(
                curva_atrito,
                "Atrito e encunhamento",
                "Tensão após atrito",
                "x (m)",
                "Tensão no cabo (MPa)",
                470.0f,
                &curva_encunhamento,
                "Tensão após encunhamento"
            );

            ImGui::Spacing();
            ImGui::Text("Tabela de perda de tensão por atrito");
            desenharTabelaAtrito(viga);

            ImGui::Spacing();
            ImGui::Text("Tabela de perda de tensão por encunhamento");
            desenharTabelaEncunhamento(viga);
        }
        else if (pagina == 3) {
            ImGui::Text("Perda imediata por encurtamento elástico");

            ImGui::Spacing();
            plotarPontos(
                curva_encurtamento,
                "Encurtamento elástico",
                "Tensão após encurtamento elástico",
                "x (m)",
                "Tensão no cabo (MPa)",
                470.0f,
                0,
                ""
            );

            ImGui::Spacing();
            ImGui::Text("Tabela de encurtamento elástico");
            desenharTabelaEncurtamento(viga);
        }
        else if (pagina == 4) {
            ImGui::Text(
                "Ato da protensão: %s",
                viga.ato_protensao_aprovado ? "APROVADO" : "REPROVADO"
            );
            ImGui::Text(
                "Limites: compressão = %.2f MPa | tração = %.2f MPa",
                viga.limite_compressao_ato_MPa,
                viga.limite_tracao_ato_MPa
            );

            ImGui::Spacing();
            plotarAtoProtensao(viga, 470.0f);

            ImGui::Spacing();
            ImGui::Text("Tabela de verificação no ato da protensão");
            desenharTabelaAto(viga);
        }
        else if (pagina == 5) {
            ImGui::Text(
                "Perdas progressivas: perda máxima = %.2f MPa | perda percentual máxima = %.2f%%",
                viga.maior_perda_progressiva_total_MPa,
                viga.maior_percentual_perda_total * 100.0
            );
            ImGui::Text(
                "Coeficientes globais: φ total = %.3f | χ∞ = %.3f%% | εcs∞(t0) = %.6f | idade fictícia = %.2f dias",
                viga.perdas_progressivas.phi_total,
                viga.perdas_progressivas.coeficiente_fluencia_aco_qui_infinito_percento,
                viga.perdas_progressivas.retracao_concreto_infinito_t0,
                viga.perdas_progressivas.idade_ficticia
            );

            ImGui::Spacing();
            plotarPerdasProgressivas(viga, 470.0f);

            ImGui::Spacing();
            ImGui::Text("Tabela de perdas progressivas");
            desenharTabelaPerdasProgressivas(viga);
        }
        else if (pagina == 6) {
            ImGui::Text(
                "ELS-F: %s",
                viga.verificacoes_els_f_aprovadas ? "APROVADO" : "REPROVADO"
            );
            ImGui::Text(
                "Combinação: %s | psi = %.2f | limite de compressão = %.2f MPa | limite de tração = %.2f MPa",
                NomeCombinacaoServico(viga.combinacao_els_f).c_str(),
                viga.fator_combinacao_els_f,
                viga.limite_compressao_els_f_MPa,
                viga.limite_tracao_fctf_MPa
            );

            ImGui::Spacing();
            plotarELSF(viga, 470.0f);

            ImGui::Spacing();
            ImGui::Text("Tabela de verificação do ELS-F");
            desenharTabelaELSF(viga);
        }
        else if (pagina == 7) {
            ImGui::Text(
                "ELS-D: %s",
                viga.verificacoes_els_d_aprovadas ? "APROVADO" : "REPROVADO"
            );
            ImGui::Text(
                "Combinação: %s | psi = %.2f | limite de compressão = %.2f MPa | limite de tração = %.2f MPa",
                NomeCombinacaoServico(viga.combinacao_els_d).c_str(),
                viga.fator_combinacao_els_d,
                viga.limite_compressao_els_d_MPa,
                viga.limite_tracao_els_d_MPa
            );

            ImGui::Spacing();
            plotarELSD(viga, 470.0f);

            ImGui::Spacing();
            ImGui::Text("Tabela de verificação do ELS-D");
            desenharTabelaELSD(viga);
        }
        else if (pagina == 8) {
            double as_min = CalcularAsMinimaGraficaCm2(viga);

            ImGui::Text(
                "ELU: %s | Msd crítico = %.2f kN.m | As,nec máxima = %.2f cm2 | As,min gráfica = %.2f cm2",
                viga.verificacoes_elu_aprovadas ? "CALCULADO" : "ERRO",
                viga.maior_momento_solicitante_elu_kNm,
                viga.maior_As_elu * 1.0e4,
                as_min
            );
            ImGui::Text(
                "As,min é exibida como referência de 0,15%% da área bruta e não altera o dimensionamento existente."
            );

            ImGui::Spacing();
            plotarELU(viga, 500.0f);

            ImGui::Spacing();
            plotarAsELU(viga, 500.0f);

            ImGui::Spacing();
            ImGui::Text("Tabela de dimensionamento no ELU");
            desenharTabelaELU(viga);
        }
    }
};

VigaConcretoProtendido GerarVigaAleatoria()
{
    double fck = GerarAleatorioArredondado(20.0, 90.0, 5.0);
    TipoCimento tipo_cimento = static_cast<TipoCimento>(GerarInteiroAleatorio(0, 4));
    TipoAgregado tipo_agregado = static_cast<TipoAgregado>(GerarInteiroAleatorio(0, 3));
    Concreto concreto(fck, tipo_cimento, tipo_agregado);

    double fyk = GerarAleatorioArredondado(400.0, 600.0, 10.0);
    double modulo_elasticidade_aco = GerarAleatorioArredondado(190.0, 210.0, 10.0);
    AcoPassivo aco(fyk, modulo_elasticidade_aco);

    ClasseAgressividadeAmbiental caa = static_cast<ClasseAgressividadeAmbiental>(GerarInteiroAleatorio(0, 3));

    TipoEstrutura estrutura = Concreto_Protendido;
    ElementoEstrutural elemento_estrutural = Viga;

    double fi_t = GerarAleatorioArredondado(8.0, 16.0, 1.0);
    double fi_l = GerarAleatorioArredondado(5.0, 12.5, 0.5);
    ArmaduraPassiva armadura_passiva(aco, fi_l, fi_t, caa, estrutura, elemento_estrutural);

    TipoAcoAtivo tipo_aco_ativo = static_cast<TipoAcoAtivo>(GerarInteiroAleatorio(0, 3));
    TipoProtensao tipo_protensao = static_cast<TipoProtensao>(GerarInteiroAleatorio(0, 3));

    double fptk = GerarAleatorioArredondado(1700.0, 2100.0, 100.0);
    double fator_fpyk = GerarAleatorioArredondado(0.8, 1.0, 0.1);
    double modulo_E_aco_prot = GerarAleatorioArredondado(190.0, 210.0, 10.0);
    AcoAtivo aco_cp(fptk, fator_fpyk, modulo_E_aco_prot, tipo_aco_ativo, tipo_protensao);

    double area_cordoalha_cm2 = GerarAleatorioArredondado(0.7, 1.5, 0.01);
    int numero_cordoalhas = GerarInteiroAleatorio(2, 15);
    TipoCaboeProtensao tipo_cabo_e_protensao = static_cast<TipoCaboeProtensao>(GerarInteiroAleatorio(0, 4));
    NivelProtensao nivel_protensao = static_cast<NivelProtensao>(GerarInteiroAleatorio(1, 2));
    ArmaduraAtiva armadura_ativa(aco_cp, numero_cordoalhas, area_cordoalha_cm2, tipo_cabo_e_protensao, nivel_protensao);

    double h = GerarAleatorioArredondado(40.0, 120.0, 5.0);
    double b = GerarAleatorioArredondado(20.0, 60.0, 5.0);
    SecaoRetangular secao_viga(h, b);

    double umidade = GerarAleatorioArredondado(40.0, 90.0, 5.0);
    double temperatura = GerarAleatorioArredondado(10.0, 35.0, 1.0);
    CondicoesClima condicoes_clima(temperatura, umidade);

    double altura_do_cabo_no_apoio_cm = GerarAleatorioArredondado(0.45 * h, 0.65 * h, 5.0);
    double excentricidade_max_cabo_cm = GerarAleatorioArredondado(5.0, 0.35 * h, 5.0);

    int numero_pontos = GerarInteiroAleatorio(11, 41);

    double escorregamento_ancoragem_mm = GerarAleatorioArredondado(2.0, 10.0, 1.0);

    bool usar_tracado_pedro = false;

    VigaConcretoProtendido::Cabo cabo(
        armadura_ativa,
        altura_do_cabo_no_apoio_cm,
        excentricidade_max_cabo_cm,
        numero_pontos,
        escorregamento_ancoragem_mm,
        usar_tracado_pedro
    );

    double vao = GerarAleatorioArredondado(5.0, 30.0, 0.5);
    double NSd_kN = GerarAleatorioArredondado(-200.0, 200.0, 10.0);
    double qSd_kN_por_m = GerarAleatorioArredondado(1.0, 50.0, 1.0);

    int dias_ato_protensao = GerarInteiroAleatorio(3, 90);

    VigaConcretoProtendido viga(
        vao,
        0.0,
        NSd_kN,
        qSd_kN_por_m,
        concreto,
        armadura_passiva,
        secao_viga,
        cabo,
        condicoes_clima,
        dias_ato_protensao
    );

    viga.tipo_acao_variavel = static_cast<TipoAcaoVariavel>(GerarInteiroAleatorio(0, 4));

    return viga;
}

void ExportarResultadosVigas(
    std::vector<VigaConcretoProtendido>& vigas,
    std::string nome_arquivo
)
{
    std::ofstream arquivo(nome_arquivo);

    if (!arquivo.is_open()) {
        std::cout << "Erro ao criar o arquivo: " << nome_arquivo << std::endl;
        return;
    }

    arquivo << std::fixed << std::setprecision(4);

    int quantidade_aprovadas = 0;

    for (int i = 0; i < (int)vigas.size(); i++) {
        if (vigas[i].verificacoes_els_aprovadas) {
            quantidade_aprovadas++;
        }
    }

    arquivo << "RELATÓRIO DE VIGAS DE CONCRETO PROTENDIDO\n";
    arquivo << "Quantidade total: " << vigas.size() << "\n";
    arquivo << "Aprovadas no ELS: " << quantidade_aprovadas << "\n";
    arquivo << "Reprovadas no ELS: " << (int)vigas.size() - quantidade_aprovadas << "\n\n";

    for (int i = 0; i < (int)vigas.size(); i++) {
        VigaConcretoProtendido& viga = vigas[i];
        double area_ativa = viga.cabo.armadura_ativa.area_total;
        double as_min_cm2 = CalcularAsMinimaGraficaCm2(viga);

        arquivo << "============================================================\n";
        arquivo << "VIGA " << i + 1 << "\n";
        arquivo << "============================================================\n\n";

        arquivo << "1. RESUMO GERAL\n";
        arquivo << "ELS geral: " << (viga.verificacoes_els_aprovadas ? "APROVADO" : "REPROVADO") << "\n";
        arquivo << "Ato da protensão: " << (viga.ato_protensao_aprovado ? "APROVADO" : "REPROVADO") << "\n";
        arquivo << "ELS-F: " << (viga.verificacoes_els_f_aprovadas ? "APROVADO" : "REPROVADO") << "\n";
        arquivo << "ELS-D: " << (viga.verificacoes_els_d_aprovadas ? "APROVADO" : "REPROVADO") << "\n";
        arquivo << "ELU: " << (viga.verificacoes_elu_aprovadas ? "CALCULADO" : "ERRO NO CÁLCULO") << "\n";
        arquivo << "Nível de protensão: " << NomeNivelProtensao(viga.cabo.armadura_ativa.nivel_protensao) << "\n";
        arquivo << "Vão = " << viga.vao << " m\n";
        arquivo << "NSd = " << viga.NSd / 1000.0 << " kN\n";
        arquivo << "qk = " << viga.qk / 1000.0 << " kN/m\n";
        arquivo << "Peso próprio = " << viga.peso_proprio_por_metro / 1000.0 << " kN/m\n";
        arquivo << "Msd crítico = " << viga.maior_momento_solicitante_elu_kNm << " kN.m\n";
        arquivo << "As,nec máxima = " << viga.maior_As_elu * 1.0e4 << " cm2\n";
        arquivo << "As,min gráfica = " << as_min_cm2 << " cm2 (0,15% Ac)\n";
        arquivo << "Observação: As,min é somente referência gráfica e não altera o cálculo existente.\n\n";

        arquivo << "2. DADOS GERAIS DA VIGA\n";
        arquivo << "fck = " << viga.concreto.fck_MPa << " MPa\n";
        arquivo << "fctm = " << viga.concreto.fctm_MPa << " MPa\n";
        arquivo << "Eci = " << viga.concreto.modulo_elasticidade_inicial << " MPa\n";
        arquivo << "Ecs = " << viga.concreto.modulo_elasticidade_secante << " MPa\n";
        arquivo << "Altura da seção = " << viga.secao.altura << " m\n";
        arquivo << "Largura da seção = " << viga.secao.largura << " m\n";
        arquivo << "Área da seção = " << viga.secao.area << " m2\n";
        arquivo << "Inércia x = " << viga.secao.inercia_x << " m4\n";
        arquivo << "Altura útil = " << viga.secao.altura_util << " m\n";
        arquivo << "Número de cordoalhas = " << viga.cabo.armadura_ativa.numero_cordoalhas << "\n";
        arquivo << "Área total ativa = " << area_ativa * 1.0e4 << " cm2\n";
        arquivo << "Tensão inicial = " << viga.cabo.armadura_ativa.tensao_inicial / 1.0e6 << " MPa\n";
        arquivo << "Força inicial = " << viga.cabo.armadura_ativa.forca_inicial / 1000.0 << " kN\n";
        arquivo << "Força final = " << viga.cabo.armadura_ativa.forca_final / 1000.0 << " kN\n\n";

        arquivo << "3. DADOS DO GRÁFICO E TABELA - PERDA POR ATRITO\n";
        arquivo << "Trecho; x (m); h(i-1) (cm); h(i) (cm); Δh(i) (cm); β (rad); α (rad); ";
        arquivo << "Σα (rad); ΔP atr (kN); σp após atrito (MPa)\n";

        for (int j = 0; j < (int)viga.cabo.pontos.size(); j++) {
            double h_inicial_cm = viga.cabo.pontos[j].y * 100.0;
            double h_final_cm = h_inicial_cm;
            double delta_h_cm = 0.0;
            double beta = 0.0;
            double alfa = 0.0;
            double alfa_acumulado = 0.0;
            double perda_forca_atrito_kN = 0.0;

            if (j > 0 && j - 1 < (int)viga.cabo.trechos.size()) {
                const auto& trecho = viga.cabo.trechos[j - 1];
                h_inicial_cm = trecho.altura_inicial * 100.0;
                h_final_cm = trecho.altura_final * 100.0;
                delta_h_cm = trecho.dy * 100.0;
                beta = trecho.beta;
                alfa = trecho.alfa;
                alfa_acumulado = trecho.alfa_acumulado;
                perda_forca_atrito_kN = trecho.perda_forca_atrito / 1000.0;
            }

            arquivo << (j == 0 ? "0" : std::to_string(j - 1) + "-" + std::to_string(j)) << "; ";
            arquivo << viga.cabo.pontos[j].x << "; ";
            arquivo << h_inicial_cm << "; ";
            arquivo << h_final_cm << "; ";
            arquivo << delta_h_cm << "; ";
            arquivo << beta << "; ";
            arquivo << alfa << "; ";
            arquivo << alfa_acumulado << "; ";
            arquivo << perda_forca_atrito_kN << "; ";
            arquivo << viga.cabo.pontos[j].tensao_apos_atrito_MPa << "\n";
        }

        arquivo << "\n4. DADOS DO GRÁFICO E TABELA - PERDA POR ENCUNHAMENTO\n";
        arquivo << "Ponto; Trecho; x (m); ep (m); σp antes do encunhamento (MPa); ";
        arquivo << "Δσp,enc (MPa); σp após encunhamento (MPa)\n";

        for (int j = 0; j < (int)viga.cabo.pontos.size(); j++) {
            const auto& ponto = viga.cabo.pontos[j];
            double perda_encunhamento =
                ponto.tensao_apos_atrito_MPa
                - ponto.tensao_apos_encunhamento_MPa;

            arquivo << j << "; ";
            arquivo << (j == 0 ? "0" : std::to_string(j - 1) + "-" + std::to_string(j)) << "; ";
            arquivo << ponto.x << "; ";
            arquivo << ponto.excentricidade << "; ";
            arquivo << ponto.tensao_apos_atrito_MPa << "; ";
            arquivo << perda_encunhamento << "; ";
            arquivo << ponto.tensao_apos_encunhamento_MPa << "\n";
        }

        arquivo << "\n5. DADOS DO GRÁFICO E TABELA - ENCURTAMENTO ELÁSTICO\n";
        arquivo << "Trecho; x (m); Mpp (kN.m); ep (m); P0 (kN); ";
        arquivo << "σcp + σcg (MPa); Δσp,enc (MPa); σp após encurtamento elástico (MPa)\n";

        for (int j = 0; j < (int)viga.cabo.pontos.size(); j++) {
            const auto& ponto = viga.cabo.pontos[j];

            arquivo << (j == 0 ? "0" : std::to_string(j - 1) + "-" + std::to_string(j)) << "; ";
            arquivo << ponto.x << "; ";
            arquivo << ponto.momento_peso_proprio_kNm << "; ";
            arquivo << ponto.excentricidade << "; ";
            arquivo << ponto.forca_protensao_kN << "; ";
            arquivo << ponto.tensao_concreto_no_nivel_do_cabo_MPa << "; ";
            arquivo << ponto.perda_encurtamento_MPa << "; ";
            arquivo << ponto.tensao_apos_encurtamento_MPa << "\n";
        }

        arquivo << "\n6. DADOS DO GRÁFICO E TABELA - ATO DA PROTENSÃO\n";
        arquivo << "Trecho; x (m); Pi · γp (kN); ep (m); Mpp (kN.m); ";
        arquivo << "σi (topo) (MPa); σi (base) (MPa)\n";

        for (int j = 0; j < (int)viga.cabo.pontos.size(); j++) {
            const auto& ponto = viga.cabo.pontos[j];

            arquivo << (j == 0 ? "0" : std::to_string(j - 1) + "-" + std::to_string(j)) << "; ";
            arquivo << ponto.x << "; ";
            arquivo << ponto.forca_protensao_ato_kN << "; ";
            arquivo << ponto.excentricidade << "; ";
            arquivo << ponto.momento_peso_proprio_ato_kNm << "; ";
            arquivo << ponto.tensao_topo_ato_MPa << "; ";
            arquivo << ponto.tensao_base_ato_MPa << "\n";
        }

        arquivo << "\n7. DADOS DO GRÁFICO E TABELA - PERDAS PROGRESSIVAS\n";
        arquivo << "Perda progressiva máxima (MPa); "
                << viga.maior_perda_progressiva_total_MPa << "\n";
        arquivo << "Perda percentual máxima (%); "
                << viga.maior_percentual_perda_total * 100.0 << "\n";
        arquivo << "Idade fictícia (dias); "
                << viga.perdas_progressivas.idade_ficticia << "\n";
        arquivo << "φ total; "
                << viga.perdas_progressivas.phi_total << "\n";
        arquivo << "χ∞ (%); "
                << viga.perdas_progressivas.coeficiente_fluencia_aco_qui_infinito_percento << "\n";
        arquivo << "εcs∞; "
                << viga.perdas_progressivas.retracao_concreto_infinito << "\n";
        arquivo << "εcs∞(t0); "
                << viga.perdas_progressivas.retracao_concreto_infinito_t0 << "\n";

        arquivo << "Trecho; x (m); ep (m); η; εcs∞; εcs∞(t0); φ total; χ∞ (%); ";
        arquivo << "Δσp prog (MPa); σp após perdas prog (MPa)\n";

        for (int j = 0; j < (int)viga.cabo.pontos.size(); j++) {
            const auto& ponto = viga.cabo.pontos[j];
            const auto& perdas = viga.perdas_progressivas;

            arquivo << (j == 0 ? "0" : std::to_string(j - 1) + "-" + std::to_string(j)) << "; ";
            arquivo << ponto.x << "; ";
            arquivo << ponto.excentricidade << "; ";
            arquivo << ponto.eta << "; ";
            arquivo << perdas.retracao_concreto_infinito << "; ";
            arquivo << perdas.retracao_concreto_infinito_t0 << "; ";
            arquivo << perdas.phi_total << "; ";
            arquivo << perdas.coeficiente_fluencia_aco_qui_infinito_percento << "; ";
            arquivo << ponto.perda_progressiva_total_MPa << "; ";
            arquivo << ponto.tensao_apos_perda_progressiva__MPa << "\n";
        }

        arquivo << "\n8. DADOS DO GRÁFICO E TABELA - ELS-F\n";
        arquivo << "Combinação; " << NomeCombinacaoServico(viga.combinacao_els_f)
                << "; ψ; " << viga.fator_combinacao_els_f << "\n";
        arquivo << "Trecho; x (m); P∞ (kN); ep (m); Mpp (kN.m); Mq (kN.m); ";
        arquivo << "MCF (kN.m); σCF (topo) (MPa); σCF (base) (MPa)\n";

        for (int j = 0; j < (int)viga.cabo.pontos.size(); j++) {
            const auto& ponto = viga.cabo.pontos[j];

            arquivo << (j == 0 ? "0" : std::to_string(j - 1) + "-" + std::to_string(j)) << "; ";
            arquivo << ponto.x << "; ";
            arquivo << ponto.forca_protensao_els_f_kN << "; ";
            arquivo << ponto.excentricidade << "; ";
            arquivo << ponto.momento_peso_proprio_kNm << "; ";
            arquivo << ponto.momento_devido_carga_kNm << "; ";
            arquivo << ponto.momento_els_f_kNm << "; ";
            arquivo << ponto.tensao_topo_els_f_MPa << "; ";
            arquivo << ponto.tensao_base_els_f_MPa << "\n";
        }

        arquivo << "\n9. DADOS DO GRÁFICO E TABELA - ELS-D\n";
        arquivo << "Combinação; " << NomeCombinacaoServico(viga.combinacao_els_d)
                << "; ψ; " << viga.fator_combinacao_els_d << "\n";
        arquivo << "Trecho; x (m); P∞ (kN); ep (m); Mpp (kN.m); Mq (kN.m); ";
        arquivo << "MCQP (kN.m); σCQP (topo) (MPa); σCQP (base) (MPa)\n";

        for (int j = 0; j < (int)viga.cabo.pontos.size(); j++) {
            const auto& ponto = viga.cabo.pontos[j];

            arquivo << (j == 0 ? "0" : std::to_string(j - 1) + "-" + std::to_string(j)) << "; ";
            arquivo << ponto.x << "; ";
            arquivo << ponto.forca_protensao_els_d_kN << "; ";
            arquivo << ponto.excentricidade << "; ";
            arquivo << ponto.momento_peso_proprio_kNm << "; ";
            arquivo << ponto.momento_devido_carga_kNm << "; ";
            arquivo << ponto.momento_els_d_kNm << "; ";
            arquivo << ponto.tensao_topo_els_d_MPa << "; ";
            arquivo << ponto.tensao_base_els_d_MPa << "\n";
        }

        arquivo << "\n10. DADOS DOS GRÁFICOS E TABELA - ELU\n";
        arquivo << "Ponto; Trecho; x (m); ep (m); P efetiva (kN); P ELU (kN); ";
        arquivo << "Δσp (MPa); Mpp (kN.m); Mq (kN.m); Msd (kN.m); x/d; ";
        arquivo << "As,nec (cm2); As,min (cm2); Status\n";

        for (int j = 0; j < (int)viga.cabo.pontos.size(); j++) {
            const auto& ponto = viga.cabo.pontos[j];
            double as_nec = ponto.As_elu * 1.0e4;

            arquivo << j << "; ";
            arquivo << (j == 0 ? "0" : std::to_string(j - 1) + "-" + std::to_string(j)) << "; ";
            arquivo << ponto.x << "; ";
            arquivo << ponto.excentricidade << "; ";
            arquivo << ponto.forca_apos_perda_progressiva_kN << "; ";
            arquivo << ponto.forca_protensao_elu_kN << "; ";
            arquivo << ponto.delta_sigma_p_elu_MPa << "; ";
            arquivo << ponto.momento_peso_proprio_elu_kNm << "; ";
            arquivo << ponto.momento_carga_elu_kNm << "; ";
            arquivo << ponto.momento_solicitante_elu_kNm << "; ";
            arquivo << ponto.x_sobre_d_elu << "; ";
            arquivo << as_nec << "; ";
            arquivo << as_min_cm2 << "; ";
            arquivo << (ponto.elu_aprovado_no_ponto ? "OK" : "ERRO") << "\n";
        }

        arquivo << "\n\n";
    }

    arquivo.close();
}



std::string FormatarNumeroCSV(double valor, int casas_decimais)
{
    std::ostringstream texto;
    texto << std::fixed << std::setprecision(casas_decimais) << valor;

    std::string resultado = texto.str();

    for (char& caractere : resultado) {
        if (caractere == '.') {
            caractere = ',';
        }
    }

    return resultado;
}

std::string FormatarTrechoCSV(int indice)
{
    std::string trecho;

    if (indice == 0) {
        trecho = "0";
    }
    else {
        trecho =
            std::to_string(indice - 1)
            + "-"
            + std::to_string(indice);
    }

    // A fórmula de texto impede que o Excel converta "1-2" em data.
    return "=\"" + trecho + "\"";
}


std::u16string ConverterUTF8ParaUTF16(const std::string& texto_utf8)
{
    std::wstring_convert<
        std::codecvt_utf8_utf16<char16_t>,
        char16_t
    > conversor;

    return conversor.from_bytes(texto_utf8);
}

bool SalvarCSVComoUTF16LE(
    const std::string& nome_arquivo,
    const std::string& conteudo_utf8
)
{
    std::ofstream arquivo_saida(
        nome_arquivo,
        std::ios::binary | std::ios::trunc
    );

    if (!arquivo_saida.is_open()) {
        return false;
    }

    // BOM UTF-16 LE. O Excel para Windows reconhece esta codificação
    // de forma mais confiável que UTF-8 em arquivos abertos diretamente no Excel.
    const unsigned char bom_utf16_le[2] = {0xFF, 0xFE};

    arquivo_saida.write(
        reinterpret_cast<const char*>(bom_utf16_le),
        2
    );

    std::u16string conteudo_utf16 =
        ConverterUTF8ParaUTF16(conteudo_utf8);

    for (char16_t caractere : conteudo_utf16) {
        char bytes[2];

        bytes[0] =
            static_cast<char>(caractere & 0x00FF);

        bytes[1] =
            static_cast<char>((caractere >> 8) & 0x00FF);

        arquivo_saida.write(bytes, 2);
    }

    return arquivo_saida.good();
}

bool ExportarResultadosVigaCSV(
    VigaConcretoProtendido& viga,
    const std::string& nome_arquivo
)
{
    // O conteúdo é montado primeiro em UTF-8 e salvo ao final como
    // UTF-16 LE, evitando textos como "VerificaÃ§Ã£o" no Excel.
    std::ostringstream arquivo;
    arquivo.imbue(std::locale::classic());

    // Em UTF-16 LE, o Excel reconhece TAB como separador de colunas.
    // Isso permite abrir o arquivo diretamente, sem importação manual.

    double area_ativa =
        viga.cabo.armadura_ativa.area_total;

    double as_min_cm2 =
        CalcularAsMinimaGraficaCm2(viga);

    // ---------------------------------------------------------------
    // 1. Resumo
    // ---------------------------------------------------------------
    arquivo << "RESUMO GERAL\r\n";
    arquivo << "Verificação\tResultado\tParâmetro\tValor\tUnidade\tObservação\r\n";

    arquivo
        << "ELS geral\t"
        << (viga.verificacoes_els_aprovadas ? "APROVADO" : "REPROVADO")
        << "\tVão\t"
        << FormatarNumeroCSV(viga.vao, 2)
        << "\tm\t\r\n";

    arquivo
        << "Ato da protensão\t"
        << (viga.ato_protensao_aprovado ? "APROVADO" : "REPROVADO")
        << "\tqk\t"
        << FormatarNumeroCSV(viga.qk / 1000.0, 2)
        << "\tkN/m\t\r\n";

    arquivo
        << "ELS-F\t"
        << (viga.verificacoes_els_f_aprovadas ? "APROVADO" : "REPROVADO")
        << "\tMsd crítico\t"
        << FormatarNumeroCSV(
            viga.maior_momento_solicitante_elu_kNm,
            2
        )
        << "\tkN.m\t\r\n";

    arquivo
        << "ELS-D\t"
        << (viga.verificacoes_els_d_aprovadas ? "APROVADO" : "REPROVADO")
        << "\tAs,nec máxima\t"
        << FormatarNumeroCSV(
            viga.maior_As_elu * 1.0e4,
            2
        )
        << "\tcm²\t\r\n";

    arquivo
        << "ELU\t"
        << (viga.verificacoes_elu_aprovadas ? "CALCULADO" : "ERRO")
        << "\tAs,min gráfica\t"
        << FormatarNumeroCSV(as_min_cm2, 2)
        << "\tcm²\tReferência gráfica de 0,15% Ac\r\n\r\n";

    // ---------------------------------------------------------------
    // 2. Dados gerais
    // ---------------------------------------------------------------
    arquivo << "DADOS GERAIS DA VIGA\r\n";
    arquivo << "Grupo\tParâmetro\tValor\tUnidade\r\n";

    arquivo
        << "Geometria\tVão\t"
        << FormatarNumeroCSV(viga.vao, 2)
        << "\tm\r\n";

    arquivo
        << "Geometria\tAltura da seção\t"
        << FormatarNumeroCSV(viga.secao.altura, 4)
        << "\tm\r\n";

    arquivo
        << "Geometria\tLargura da seção\t"
        << FormatarNumeroCSV(viga.secao.largura, 4)
        << "\tm\r\n";

    arquivo
        << "Geometria\tÁrea da seção\t"
        << FormatarNumeroCSV(viga.secao.area, 4)
        << "\tm²\r\n";

    arquivo
        << "Geometria\tInércia x\t"
        << FormatarNumeroCSV(viga.secao.inercia_x, 5)
        << "\tm⁴\r\n";

    arquivo
        << "Materiais\tfck\t"
        << FormatarNumeroCSV(viga.concreto.fck_MPa, 2)
        << "\tMPa\r\n";

    arquivo
        << "Materiais\tfctm\t"
        << FormatarNumeroCSV(viga.concreto.fctm_MPa, 3)
        << "\tMPa\r\n";

    arquivo
        << "Materiais\tEci\t"
        << FormatarNumeroCSV(
            viga.concreto.modulo_elasticidade_inicial,
            2
        )
        << "\tMPa\r\n";

    arquivo
        << "Materiais\tEcs\t"
        << FormatarNumeroCSV(
            viga.concreto.modulo_elasticidade_secante,
            2
        )
        << "\tMPa\r\n";

    arquivo
        << "Armadura ativa\tNúmero de cordoalhas\t"
        << viga.cabo.armadura_ativa.numero_cordoalhas
        << "\t-\r\n";

    arquivo
        << "Armadura ativa\tÁrea total ativa\t"
        << FormatarNumeroCSV(area_ativa * 1.0e4, 2)
        << "\tcm²\r\n";

    arquivo
        << "Armadura ativa\tTensão inicial\t"
        << FormatarNumeroCSV(
            viga.cabo.armadura_ativa.tensao_inicial / 1.0e6,
            2
        )
        << "\tMPa\r\n";

    arquivo
        << "Armadura ativa\tForça inicial\t"
        << FormatarNumeroCSV(
            viga.cabo.armadura_ativa.forca_inicial / 1000.0,
            2
        )
        << "\tkN\r\n";

    arquivo
        << "Armadura ativa\tForça final\t"
        << FormatarNumeroCSV(
            viga.cabo.armadura_ativa.forca_final / 1000.0,
            2
        )
        << "\tkN\r\n";

    arquivo
        << "Solicitações\tNSd\t"
        << FormatarNumeroCSV(viga.NSd / 1000.0, 2)
        << "\tkN\r\n";

    arquivo
        << "Solicitações\tqk\t"
        << FormatarNumeroCSV(viga.qk / 1000.0, 2)
        << "\tkN/m\r\n";

    arquivo
        << "Solicitações\tPeso próprio\t"
        << FormatarNumeroCSV(
            viga.peso_proprio_por_metro / 1000.0,
            2
        )
        << "\tkN/m\r\n\r\n";

    // ---------------------------------------------------------------
    // 3. Atrito
    // ---------------------------------------------------------------
    arquivo << "PERDA DE TENSÃO POR ATRITO\r\n";
    arquivo
        << "Trecho\tx (m)\th(i-1) (cm)\th(i) (cm)\tΔh(i) (cm)\t"
        << "β (rad)\tα (rad)\tΣα (rad)\tΔP atr (kN)\t"
        << "σp após atrito (MPa)\r\n";

    for (int i = 0; i < (int)viga.cabo.pontos.size(); i++) {
        double h_inicial_cm =
            viga.cabo.pontos[i].y * 100.0;

        double h_final_cm = h_inicial_cm;
        double delta_h_cm = 0.0;
        double beta = 0.0;
        double alfa = 0.0;
        double alfa_acumulado = 0.0;
        double perda_forca_atrito_kN = 0.0;

        if (
            i > 0
            && i - 1 < (int)viga.cabo.trechos.size()
        ) {
            const auto& trecho =
                viga.cabo.trechos[i - 1];

            h_inicial_cm =
                trecho.altura_inicial * 100.0;

            h_final_cm =
                trecho.altura_final * 100.0;

            delta_h_cm =
                trecho.dy * 100.0;

            beta = trecho.beta;
            alfa = trecho.alfa;
            alfa_acumulado = trecho.alfa_acumulado;

            perda_forca_atrito_kN =
                trecho.perda_forca_atrito / 1000.0;
        }

        arquivo
            << FormatarTrechoCSV(i) << "\t"
            << FormatarNumeroCSV(viga.cabo.pontos[i].x, 4) << "\t"
            << FormatarNumeroCSV(h_inicial_cm, 2) << "\t"
            << FormatarNumeroCSV(h_final_cm, 2) << "\t"
            << FormatarNumeroCSV(delta_h_cm, 2) << "\t"
            << FormatarNumeroCSV(beta, 4) << "\t"
            << FormatarNumeroCSV(alfa, 4) << "\t"
            << FormatarNumeroCSV(alfa_acumulado, 4) << "\t"
            << FormatarNumeroCSV(perda_forca_atrito_kN, 2) << "\t"
            << FormatarNumeroCSV(
                viga.cabo.pontos[i].tensao_apos_atrito_MPa,
                2
            )
            << "\r\n";
    }

    arquivo << "\r\n";

    // ---------------------------------------------------------------
    // 4. Encunhamento
    // ---------------------------------------------------------------
    arquivo << "PERDA DE TENSÃO POR ENCUNHAMENTO\r\n";
    arquivo
        << "Ponto\tTrecho\tx (m)\tep (m)\t"
        << "σp antes do encunhamento (MPa)\t"
        << "Δσp,enc (MPa)\t"
        << "σp após encunhamento (MPa)\r\n";

    for (int i = 0; i < (int)viga.cabo.pontos.size(); i++) {
        const auto& ponto = viga.cabo.pontos[i];

        double perda_encunhamento =
            ponto.tensao_apos_atrito_MPa
            - ponto.tensao_apos_encunhamento_MPa;

        arquivo
            << i << "\t"
            << FormatarTrechoCSV(i) << "\t"
            << FormatarNumeroCSV(ponto.x, 4) << "\t"
            << FormatarNumeroCSV(ponto.excentricidade, 4) << "\t"
            << FormatarNumeroCSV(
                ponto.tensao_apos_atrito_MPa,
                2
            )
            << "\t"
            << FormatarNumeroCSV(perda_encunhamento, 2) << "\t"
            << FormatarNumeroCSV(
                ponto.tensao_apos_encunhamento_MPa,
                2
            )
            << "\r\n";
    }

    arquivo << "\r\n";

    // ---------------------------------------------------------------
    // 5. Encurtamento elástico
    // ---------------------------------------------------------------
    arquivo << "PERDA DE TENSÃO POR ENCURTAMENTO ELÁSTICO\r\n";
    arquivo
        << "Trecho\tx (m)\tMpp (kN.m)\tep (m)\tP0 (kN)\t"
        << "σcp + σcg (MPa)\tΔσp,enc (MPa)\t"
        << "σp após encurtamento elástico (MPa)\r\n";

    for (int i = 0; i < (int)viga.cabo.pontos.size(); i++) {
        const auto& ponto = viga.cabo.pontos[i];

        arquivo
            << FormatarTrechoCSV(i) << "\t"
            << FormatarNumeroCSV(ponto.x, 4) << "\t"
            << FormatarNumeroCSV(
                ponto.momento_peso_proprio_kNm,
                2
            )
            << "\t"
            << FormatarNumeroCSV(ponto.excentricidade, 4) << "\t"
            << FormatarNumeroCSV(
                ponto.forca_protensao_kN,
                2
            )
            << "\t"
            << FormatarNumeroCSV(
                ponto.tensao_concreto_no_nivel_do_cabo_MPa,
                3
            )
            << "\t"
            << FormatarNumeroCSV(
                ponto.perda_encurtamento_MPa,
                3
            )
            << "\t"
            << FormatarNumeroCSV(
                ponto.tensao_apos_encurtamento_MPa,
                2
            )
            << "\r\n";
    }

    arquivo << "\r\n";

    // ---------------------------------------------------------------
    // 6. Ato da protensão
    // ---------------------------------------------------------------
    arquivo << "VERIFICAÇÃO NO ATO DA PROTENSÃO\r\n";
    arquivo
        << "Trecho\tx (m)\tPi · γp (kN)\tep (m)\tMpp (kN.m)\t"
        << "σi (topo) (MPa)\tσi (base) (MPa)\r\n";

    for (int i = 0; i < (int)viga.cabo.pontos.size(); i++) {
        const auto& ponto = viga.cabo.pontos[i];

        arquivo
            << FormatarTrechoCSV(i) << "\t"
            << FormatarNumeroCSV(ponto.x, 4) << "\t"
            << FormatarNumeroCSV(
                ponto.forca_protensao_ato_kN,
                2
            )
            << "\t"
            << FormatarNumeroCSV(ponto.excentricidade, 4) << "\t"
            << FormatarNumeroCSV(
                ponto.momento_peso_proprio_ato_kNm,
                2
            )
            << "\t"
            << FormatarNumeroCSV(
                ponto.tensao_topo_ato_MPa,
                3
            )
            << "\t"
            << FormatarNumeroCSV(
                ponto.tensao_base_ato_MPa,
                3
            )
            << "\r\n";
    }

    arquivo << "\r\n";

    // ---------------------------------------------------------------
    // 7. Perdas progressivas
    // ---------------------------------------------------------------
    arquivo << "PERDAS PROGRESSIVAS\r\n";

    arquivo
        << "Parâmetro\tValor\tUnidade\r\n"
        << "Perda progressiva máxima\t"
        << FormatarNumeroCSV(
            viga.maior_perda_progressiva_total_MPa,
            3
        )
        << "\tMPa\r\n"
        << "Perda percentual máxima\t"
        << FormatarNumeroCSV(
            viga.maior_percentual_perda_total * 100.0,
            3
        )
        << "\t%\r\n"
        << "Idade fictícia\t"
        << FormatarNumeroCSV(
            viga.perdas_progressivas.idade_ficticia,
            3
        )
        << "\tdias\r\n"
        << "φ total\t"
        << FormatarNumeroCSV(
            viga.perdas_progressivas.phi_total,
            4
        )
        << "\t-\r\n"
        << "χ∞\t"
        << FormatarNumeroCSV(
            viga.perdas_progressivas.coeficiente_fluencia_aco_qui_infinito_percento,
            4
        )
        << "\t%\r\n"
        << "εcs∞\t"
        << FormatarNumeroCSV(
            viga.perdas_progressivas.retracao_concreto_infinito,
            8
        )
        << "\t-\r\n"
        << "εcs∞(t0)\t"
        << FormatarNumeroCSV(
            viga.perdas_progressivas.retracao_concreto_infinito_t0,
            8
        )
        << "\t-\r\n\r\n";

    arquivo
        << "Trecho\tx (m)\tep (m)\tη\tεcs∞\tεcs∞(t0)\t"
        << "φ total\tχ∞ (%)\tΔσp prog (MPa)\t"
        << "σp após perdas prog (MPa)\r\n";

    for (int i = 0; i < (int)viga.cabo.pontos.size(); i++) {
        const auto& ponto = viga.cabo.pontos[i];
        const auto& perdas = viga.perdas_progressivas;

        arquivo
            << FormatarTrechoCSV(i) << "\t"
            << FormatarNumeroCSV(ponto.x, 4) << "\t"
            << FormatarNumeroCSV(ponto.excentricidade, 4) << "\t"
            << FormatarNumeroCSV(ponto.eta, 4) << "\t"
            << FormatarNumeroCSV(
                perdas.retracao_concreto_infinito,
                8
            )
            << "\t"
            << FormatarNumeroCSV(
                perdas.retracao_concreto_infinito_t0,
                8
            )
            << "\t"
            << FormatarNumeroCSV(perdas.phi_total, 4) << "\t"
            << FormatarNumeroCSV(
                perdas.coeficiente_fluencia_aco_qui_infinito_percento,
                4
            )
            << "\t"
            << FormatarNumeroCSV(
                ponto.perda_progressiva_total_MPa,
                3
            )
            << "\t"
            << FormatarNumeroCSV(
                ponto.tensao_apos_perda_progressiva__MPa,
                3
            )
            << "\r\n";
    }

    arquivo << "\r\n";

    // ---------------------------------------------------------------
    // 8. ELS-F
    // ---------------------------------------------------------------
    arquivo << "ESTADO LIMITE DE FORMAÇÃO DE FISSURAS (ELS-F)\r\n";
    arquivo
        << "Combinação\t"
        << NomeCombinacaoServico(viga.combinacao_els_f)
        << "\tψ\t"
        << FormatarNumeroCSV(viga.fator_combinacao_els_f, 2)
        << "\r\n";

    arquivo
        << "Trecho\tx (m)\tP∞ (kN)\tep (m)\tMpp (kN.m)\t"
        << "Mq (kN.m)\tMCF (kN.m)\t"
        << "σCF (topo) (MPa)\tσCF (base) (MPa)\r\n";

    for (int i = 0; i < (int)viga.cabo.pontos.size(); i++) {
        const auto& ponto = viga.cabo.pontos[i];

        arquivo
            << FormatarTrechoCSV(i) << "\t"
            << FormatarNumeroCSV(ponto.x, 4) << "\t"
            << FormatarNumeroCSV(
                ponto.forca_protensao_els_f_kN,
                2
            )
            << "\t"
            << FormatarNumeroCSV(ponto.excentricidade, 4) << "\t"
            << FormatarNumeroCSV(
                ponto.momento_peso_proprio_kNm,
                2
            )
            << "\t"
            << FormatarNumeroCSV(
                ponto.momento_devido_carga_kNm,
                2
            )
            << "\t"
            << FormatarNumeroCSV(
                ponto.momento_els_f_kNm,
                2
            )
            << "\t"
            << FormatarNumeroCSV(
                ponto.tensao_topo_els_f_MPa,
                3
            )
            << "\t"
            << FormatarNumeroCSV(
                ponto.tensao_base_els_f_MPa,
                3
            )
            << "\r\n";
    }

    arquivo << "\r\n";

    // ---------------------------------------------------------------
    // 9. ELS-D
    // ---------------------------------------------------------------
    arquivo << "ESTADO LIMITE DE DESCOMPRESSÃO (ELS-D)\r\n";
    arquivo
        << "Combinação\t"
        << NomeCombinacaoServico(viga.combinacao_els_d)
        << "\tψ\t"
        << FormatarNumeroCSV(viga.fator_combinacao_els_d, 2)
        << "\r\n";

    arquivo
        << "Trecho\tx (m)\tP∞ (kN)\tep (m)\tMpp (kN.m)\t"
        << "Mq (kN.m)\tMCQP (kN.m)\t"
        << "σCQP (topo) (MPa)\tσCQP (base) (MPa)\r\n";

    for (int i = 0; i < (int)viga.cabo.pontos.size(); i++) {
        const auto& ponto = viga.cabo.pontos[i];

        arquivo
            << FormatarTrechoCSV(i) << "\t"
            << FormatarNumeroCSV(ponto.x, 4) << "\t"
            << FormatarNumeroCSV(
                ponto.forca_protensao_els_d_kN,
                2
            )
            << "\t"
            << FormatarNumeroCSV(ponto.excentricidade, 4) << "\t"
            << FormatarNumeroCSV(
                ponto.momento_peso_proprio_kNm,
                2
            )
            << "\t"
            << FormatarNumeroCSV(
                ponto.momento_devido_carga_kNm,
                2
            )
            << "\t"
            << FormatarNumeroCSV(
                ponto.momento_els_d_kNm,
                2
            )
            << "\t"
            << FormatarNumeroCSV(
                ponto.tensao_topo_els_d_MPa,
                3
            )
            << "\t"
            << FormatarNumeroCSV(
                ponto.tensao_base_els_d_MPa,
                3
            )
            << "\r\n";
    }

    arquivo << "\r\n";

    // ---------------------------------------------------------------
    // 10. ELU
    // ---------------------------------------------------------------
    arquivo << "DIMENSIONAMENTO NO ESTADO LIMITE ÚLTIMO (ELU)\r\n";
    arquivo
        << "Ponto\tTrecho\tx (m)\tep (m)\tP efetiva (kN)\t"
        << "P ELU (kN)\tΔσp (MPa)\tMpp (kN.m)\tMq (kN.m)\t"
        << "Msd (kN.m)\tx/d\tAs,nec (cm²)\tAs,min (cm²)\tStatus\r\n";

    for (int i = 0; i < (int)viga.cabo.pontos.size(); i++) {
        const auto& ponto = viga.cabo.pontos[i];

        arquivo
            << i << "\t"
            << FormatarTrechoCSV(i) << "\t"
            << FormatarNumeroCSV(ponto.x, 4) << "\t"
            << FormatarNumeroCSV(ponto.excentricidade, 4) << "\t"
            << FormatarNumeroCSV(
                ponto.forca_apos_perda_progressiva_kN,
                2
            )
            << "\t"
            << FormatarNumeroCSV(
                ponto.forca_protensao_elu_kN,
                2
            )
            << "\t"
            << FormatarNumeroCSV(
                ponto.delta_sigma_p_elu_MPa,
                2
            )
            << "\t"
            << FormatarNumeroCSV(
                ponto.momento_peso_proprio_elu_kNm,
                2
            )
            << "\t"
            << FormatarNumeroCSV(
                ponto.momento_carga_elu_kNm,
                2
            )
            << "\t"
            << FormatarNumeroCSV(
                ponto.momento_solicitante_elu_kNm,
                2
            )
            << "\t"
            << FormatarNumeroCSV(
                ponto.x_sobre_d_elu,
                4
            )
            << "\t"
            << FormatarNumeroCSV(
                ponto.As_elu * 1.0e4,
                2
            )
            << "\t"
            << FormatarNumeroCSV(as_min_cm2, 2)
            << "\t"
            << (
                ponto.elu_aprovado_no_ponto
                ? "OK"
                : "ERRO"
            )
            << "\r\n";
    }

    if (!SalvarCSVComoUTF16LE(
        nome_arquivo,
        arquivo.str()
    )) {
        std::cout
            << "Erro ao criar o arquivo CSV: "
            << nome_arquivo
            << std::endl;

        return false;
    }

    std::cout
        << "Arquivo CSV gerado: "
        << nome_arquivo
        << std::endl;

    return true;
}




void TestarVigasAleatorias()
{
    std::vector<VigaConcretoProtendido> vigas;

    for (int i = 0; i < 100; i++) {
        VigaConcretoProtendido viga = GerarVigaAleatoria();
        viga.dimensionar();
        vigas.push_back(viga);
    }

    ExportarResultadosVigas(vigas, "resultados_vigas_aleatorias.txt");

    std::cout << "Teste finalizado.\n";
    std::cout << "Arquivo gerado: resultados_vigas_aleatorias.txt\n";
}



enum ProdutoCordoalhaCatalogo {
    COR_CP190_RB_12_7,
    COR_CP190_RB_15_2,
    COR_CP190_RB_15_7,
    COR_CP190_RN_12_7,
    COR_CP190_RN_15_2,
    COR_CP190_RN_15_7,
    COR_CP210_RB_12_7,
    COR_CP210_RB_15_2,
    COR_CP210_RB_15_7,
    COR_CP210_RN_12_7,
    COR_CP210_RN_15_2,
    COR_CP210_RN_15_7
};

struct DadosProdutoCordoalha {
    const char* nome;
    TipoAcoAtivo tipo_aco_ativo;
    double fptk_MPa;
    double diametro_nominal_mm;
    double area_cordoalha_cm2;
    double carga_minima_ruptura_kN;
    double carga_minima_1_porcento_kN;
    double alongamento_ruptura_percento;
};

DadosProdutoCordoalha ObterDadosProdutoCordoalha(int produto)
{
    DadosProdutoCordoalha produtos[] = {
        {"CP 190 RB 12,7 mm", CP_190_RB, 1900.0, 12.7, 1.01, 184.0, 165.0, 3.5},
        {"CP 190 RB 15,2 mm", CP_190_RB, 1900.0, 15.2, 1.43, 261.0, 235.0, 3.5},
        {"CP 190 RB 15,7 mm", CP_190_RB, 1900.0, 15.7, 1.50, 274.0, 247.0, 3.5},
        {"CP 190 RN 12,7 mm", CP_190_RN, 1900.0, 12.7, 1.01, 184.0, 165.0, 3.5},
        {"CP 190 RN 15,2 mm", CP_190_RN, 1900.0, 15.2, 1.43, 261.0, 235.0, 3.5},
        {"CP 190 RN 15,7 mm", CP_190_RN, 1900.0, 15.7, 1.50, 274.0, 247.0, 3.5},
        {"CP 210 RB 12,7 mm", CP_210_RB, 2100.0, 12.7, 1.01, 203.0, 183.0, 3.5},
        {"CP 210 RB 15,2 mm", CP_210_RB, 2100.0, 15.2, 1.43, 288.0, 259.0, 3.5},
        {"CP 210 RB 15,7 mm", CP_210_RB, 2100.0, 15.7, 1.50, 303.0, 273.0, 3.5},
        {"CP 210 RN 12,7 mm", CP_210_RN, 2100.0, 12.7, 1.01, 203.0, 183.0, 3.5},
        {"CP 210 RN 15,2 mm", CP_210_RN, 2100.0, 15.2, 1.43, 288.0, 259.0, 3.5},
        {"CP 210 RN 15,7 mm", CP_210_RN, 2100.0, 15.7, 1.50, 303.0, 273.0, 3.5}
    };

    return produtos[produto];
}

struct DadosEntradaInterface {
    double fck = 30.0;
    int tipo_cimento = CP_I;
    int tipo_agregado = AGREGADO_GRANITO;

    double fyk = 500.0;
    double modulo_elasticidade_aco = 200.0;
    int caa = CAA_II;
    double diametro_barra_longitudinal_mm = 12.5;
    double diametro_estribo_mm = 5.0;

    int produto_cordoalha = COR_CP190_RB_12_7;
    int tipo_aco_ativo = CP_190_RB;
    int tipo_protensao = Pos_tracionada_nao_aderente;
    double diametro_cordoalha_mm = 12.7;
    double fptk = 1900.0;
    double fator_fpyk = 0.9;
    double modulo_E_aco_prot = 200.0;
    int numero_cordoalhas = 8;
    double area_cordoalha_cm2 = 1.01;

    double altura_secao_cm = 50.0;
    double largura_secao_cm = 30.0;

    double temperatura = 20.0;
    double umidade = 75.0;

    double altura_do_cabo_no_apoio_cm = 25.0;
    double excentricidade_max_cabo_cm = 15.0;
    int numero_pontos = 21;
    int tipo_cabo_e_protensao = Cordoalha_e_bainha_polipropileno_lubrificada;
    int nivel_protensao = Protensao_Nivel_2_Limitada;
    double escorregamento_ancoragem_mm = 6.0;
    bool usar_tracado_pedro = true;

    double vao = 17.0;
    double NSd_kN = 0.0;
    double qSd_kN_por_m = 3.0;
    int tipo_acao_variavel = Acao_Residencias_Hospedagens_Acesso_Restrito;
    int dias_ato_protensao = 10;
};

DadosEntradaInterface CriarDadosEntradaPadraoOriginal()
{
    DadosEntradaInterface dados;

    dados.fck = 30.0;
    dados.tipo_cimento = CP_I;
    dados.tipo_agregado = AGREGADO_GRANITO;

    dados.fyk = 500.0;
    dados.modulo_elasticidade_aco = 200.0;
    dados.caa = CAA_II;
    dados.diametro_barra_longitudinal_mm = 12.5;
    dados.diametro_estribo_mm = 5.0;

    dados.produto_cordoalha = COR_CP190_RB_12_7;
    dados.tipo_aco_ativo = CP_190_RB;
    dados.tipo_protensao = Pos_tracionada_nao_aderente;
    dados.diametro_cordoalha_mm = 12.7;
    dados.fptk = 1900.0;
    dados.fator_fpyk = 0.9;
    dados.modulo_E_aco_prot = 200.0;
    dados.numero_cordoalhas = 8;
    dados.area_cordoalha_cm2 = 1.01;

    dados.altura_secao_cm = 50.0;
    dados.largura_secao_cm = 30.0;

    dados.temperatura = 20.0;
    dados.umidade = 75.0;

    dados.altura_do_cabo_no_apoio_cm = 25.0;
    dados.excentricidade_max_cabo_cm = 15.0;
    dados.numero_pontos = 21;
    dados.tipo_cabo_e_protensao = Cordoalha_e_bainha_polipropileno_lubrificada;
    dados.nivel_protensao = Protensao_Nivel_2_Limitada;
    dados.escorregamento_ancoragem_mm = 6.0;
    dados.usar_tracado_pedro = true;

    dados.vao = 17.0;
    dados.NSd_kN = 0.0;
    dados.qSd_kN_por_m = 3.0;
    dados.tipo_acao_variavel = Acao_Residencias_Hospedagens_Acesso_Restrito;
    dados.dias_ato_protensao = 10;

    return dados;
}

void PreencherDadosEntradaComViga(DadosEntradaInterface& dados, VigaConcretoProtendido& viga)
{
    dados.fck = viga.concreto.fck_MPa;
    dados.tipo_cimento = (int)viga.concreto.tipo_cimento;
    dados.tipo_agregado = (int)viga.concreto.tipo_agregado;

    dados.fyk = viga.armadura_passiva.aco.fyk / 1.0e6;
    dados.modulo_elasticidade_aco = viga.armadura_passiva.aco.modulo_elasticidade_MPa / 1000.0;
    dados.caa = (int)viga.armadura_passiva.classe_agressividade_ambiental;
    dados.diametro_barra_longitudinal_mm = viga.armadura_passiva.diametro_barra_longitudinal * 1000.0;
    dados.diametro_estribo_mm = viga.armadura_passiva.diametro_estribo * 1000.0;

    dados.tipo_aco_ativo = (int)viga.cabo.armadura_ativa.aco.tipo_aco_ativo;
    dados.tipo_protensao = (int)viga.cabo.armadura_ativa.aco.tipo_protensao;
    dados.fptk = viga.cabo.armadura_ativa.aco.fptk / 1.0e6;
    dados.fator_fpyk = viga.cabo.armadura_ativa.aco.fator_fpyk;
    dados.modulo_E_aco_prot = viga.cabo.armadura_ativa.aco.modulo_elasticidade_MPa / 1000.0;
    dados.numero_cordoalhas = viga.cabo.armadura_ativa.numero_cordoalhas;
    dados.area_cordoalha_cm2 = viga.cabo.armadura_ativa.area_cordoalha * 1.0e4;

    dados.produto_cordoalha = COR_CP190_RB_12_7;
    for (int i = 0; i < 12; i++) {
        DadosProdutoCordoalha produto = ObterDadosProdutoCordoalha(i);

        if ((int)produto.tipo_aco_ativo == dados.tipo_aco_ativo
            && std::abs(produto.fptk_MPa - dados.fptk) < 0.01
            && std::abs(produto.area_cordoalha_cm2 - dados.area_cordoalha_cm2) < 0.001) {
            dados.produto_cordoalha = i;
            dados.diametro_cordoalha_mm = produto.diametro_nominal_mm;
            break;
        }
    }

    dados.altura_secao_cm = viga.secao.altura * 100.0;
    dados.largura_secao_cm = viga.secao.largura * 100.0;

    dados.temperatura = viga.condicoes_clima.temperatura_media;
    dados.umidade = viga.condicoes_clima.umidade_media;

    dados.altura_do_cabo_no_apoio_cm = viga.cabo.altura_do_cabo_no_apoio * 100.0;
    dados.excentricidade_max_cabo_cm = viga.cabo.excentricidade_max_cabo * 100.0;
    dados.numero_pontos = viga.cabo.numero_pontos;
    dados.tipo_cabo_e_protensao = (int)viga.cabo.armadura_ativa.tipo_cabo_e_protensao;
    dados.nivel_protensao = (int)viga.cabo.armadura_ativa.nivel_protensao;
    dados.escorregamento_ancoragem_mm = viga.cabo.escorregamento_ancoragem * 1000.0;
    dados.usar_tracado_pedro = viga.cabo.usar_tracado_pedro;

    dados.vao = viga.vao;
    dados.NSd_kN = viga.NSd / 1000.0;
    dados.qSd_kN_por_m = viga.qk / 1000.0;
    dados.tipo_acao_variavel = (int)viga.tipo_acao_variavel;
    dados.dias_ato_protensao = viga.n_dias_ato_protensao;
}

VigaConcretoProtendido CriarVigaPadraoOriginal()
{
    Concreto concreto_c30(30.0, CP_II, AGREGADO_GRANITO);
    AcoPassivo aco_ca50(500.0, 200.0);
    ArmaduraPassiva armadura_passiva(aco_ca50, 12.5, 5.0, CAA_II, Concreto_Protendido, Viga);
    AcoAtivo aco_cp190rb(1900.0, 0.9, 200.0, CP_190_RB, Pos_tracionada_nao_aderente);
    TipoCaboeProtensao tipo_cabo_e_protensao = Cordoalha_e_bainha_polipropileno_lubrificada;
    NivelProtensao nivel_protensao = Protensao_Nivel_2_Limitada;
    ArmaduraAtiva armadura_ativa(aco_cp190rb, 8, 1.01, tipo_cabo_e_protensao, nivel_protensao);
    SecaoRetangular secao_viga(50.0, 30.0);
    CondicoesClima condicoes_clima(20.0, 75.0);

    bool usar_tracado_pedro = true;
    VigaConcretoProtendido::Cabo cabo(armadura_ativa, 25.0, 15.0, 21, 6.0, usar_tracado_pedro);
    VigaConcretoProtendido viga(17.0, 0.0, 0.0, 3.0, concreto_c30, armadura_passiva, secao_viga, cabo, condicoes_clima, 10);
    viga.tipo_acao_variavel = Acao_Residencias_Hospedagens_Acesso_Restrito;

    return viga;
}

VigaConcretoProtendido CriarVigaPelosDadosDaInterface(DadosEntradaInterface& dados)
{
    if (dados.usar_tracado_pedro) {
        dados.numero_pontos = 21;
    }

    Concreto concreto(
        dados.fck,
        static_cast<TipoCimento>(dados.tipo_cimento),
        static_cast<TipoAgregado>(dados.tipo_agregado)
    );

    AcoPassivo aco_passivo(
        dados.fyk,
        dados.modulo_elasticidade_aco
    );

    ArmaduraPassiva armadura_passiva(
        aco_passivo,
        dados.diametro_barra_longitudinal_mm,
        dados.diametro_estribo_mm,
        static_cast<ClasseAgressividadeAmbiental>(dados.caa),
        Concreto_Protendido,
        Viga
    );

    AcoAtivo aco_ativo(
        dados.fptk,
        dados.fator_fpyk,
        dados.modulo_E_aco_prot,
        static_cast<TipoAcoAtivo>(dados.tipo_aco_ativo),
        static_cast<TipoProtensao>(dados.tipo_protensao)
    );

    ArmaduraAtiva armadura_ativa(
        aco_ativo,
        dados.numero_cordoalhas,
        dados.area_cordoalha_cm2,
        static_cast<TipoCaboeProtensao>(dados.tipo_cabo_e_protensao),
        static_cast<NivelProtensao>(dados.nivel_protensao)
    );

    SecaoRetangular secao_viga(
        dados.altura_secao_cm,
        dados.largura_secao_cm
    );

    CondicoesClima condicoes_clima(
        dados.temperatura,
        dados.umidade
    );

    VigaConcretoProtendido::Cabo cabo(
        armadura_ativa,
        dados.altura_do_cabo_no_apoio_cm,
        dados.excentricidade_max_cabo_cm,
        dados.numero_pontos,
        dados.escorregamento_ancoragem_mm,
        dados.usar_tracado_pedro
    );

    VigaConcretoProtendido viga(
        dados.vao,
        0.0,
        dados.NSd_kN,
        dados.qSd_kN_por_m,
        concreto,
        armadura_passiva,
        secao_viga,
        cabo,
        condicoes_clima,
        dados.dias_ato_protensao
    );

    viga.tipo_acao_variavel = static_cast<TipoAcaoVariavel>(dados.tipo_acao_variavel);

    return viga;
}

void DesenharPaginaEntrada(DadosEntradaInterface& dados, VigaConcretoProtendido& viga, bool& viga_dimensionada)
{

    ImGui::Separator();

    if (ImGui::Button("Dimensionar viga padrão original")) {
        dados = CriarDadosEntradaPadraoOriginal();
        viga = CriarVigaPadraoOriginal();
        viga.dimensionar();
        viga_dimensionada = true;
    }

    ImGui::Text("Ctrl + Shift + B: gerar e dimensionar viga aleatória e mostrar os dados na interface");
    ImGui::Text("Ctrl + Shift + A: calcular e exportar 100 vigas aleatórias");

    ImGui::Separator();

    if (ImGui::CollapsingHeader("Concreto", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::SetNextItemWidth(90.0f);
        ImGui::InputDouble("fck (MPa)", &dados.fck, 0.0, 0.0, "%.2f");

        static const char* tipos_cimento[] = {
            "CP I",
            "CP II",
            "CP III",
            "CP IV",
            "CP V ARI"
        };

        ImGui::SetNextItemWidth(150.0f);
        ImGui::Combo("Tipo de cimento", &dados.tipo_cimento, tipos_cimento, 5);

        static const char* tipos_agregado[] = {
            "Basalto",
            "Granito",
            "Calcario",
            "Arenito"
        };

        ImGui::SetNextItemWidth(150.0f);
        ImGui::Combo("Tipo de agregado", &dados.tipo_agregado, tipos_agregado, 4);
    }

    if (ImGui::CollapsingHeader("Armadura passiva")) {
        ImGui::SetNextItemWidth(90.0f);
        ImGui::InputDouble("fyk (MPa)", &dados.fyk, 0.0, 0.0, "%.2f");
        ImGui::SetNextItemWidth(90.0f);
        ImGui::InputDouble("E aço passivo (GPa)", &dados.modulo_elasticidade_aco, 0.0, 0.0, "%.2f");

        static const char* classes_agressividade[] = {
            "CAA I",
            "CAA II",
            "CAA III",
            "CAA IV"
        };

        ImGui::SetNextItemWidth(100.0f);
        ImGui::Combo("CAA", &dados.caa, classes_agressividade, 4);
        ImGui::SetNextItemWidth(90.0f);
        ImGui::InputDouble("Diâmetro longitudinal (mm)", &dados.diametro_barra_longitudinal_mm, 0.0, 0.0, "%.2f");
        ImGui::SetNextItemWidth(90.0f);
        ImGui::InputDouble("Diâmetro estribo (mm)", &dados.diametro_estribo_mm, 0.0, 0.0, "%.2f");
    }

    if (ImGui::CollapsingHeader("Armadura ativa", ImGuiTreeNodeFlags_DefaultOpen)) {
        static const char* produtos_cordoalha[] = {
            "CP 190 RB 12,7 mm",
            "CP 190 RB 15,2 mm",
            "CP 190 RB 15,7 mm",
            "CP 190 RN 12,7 mm",
            "CP 190 RN 15,2 mm",
            "CP 190 RN 15,7 mm",
            "CP 210 RB 12,7 mm",
            "CP 210 RB 15,2 mm",
            "CP 210 RB 15,7 mm",
            "CP 210 RN 12,7 mm",
            "CP 210 RN 15,2 mm",
            "CP 210 RN 15,7 mm"
        };

        ImGui::SetNextItemWidth(220.0f);
        if (ImGui::Combo("Produto da cordoalha", &dados.produto_cordoalha, produtos_cordoalha, 12)) {
            DadosProdutoCordoalha produto = ObterDadosProdutoCordoalha(dados.produto_cordoalha);
            dados.tipo_aco_ativo = (int)produto.tipo_aco_ativo;
            dados.fptk = produto.fptk_MPa;
            dados.diametro_cordoalha_mm = produto.diametro_nominal_mm;
            dados.area_cordoalha_cm2 = produto.area_cordoalha_cm2;
        }

        static const char* tipos_aco_ativo[] = {
            "CP 190 RB",
            "CP 190 RN",
            "CP 210 RB",
            "CP 210 RN"
        };

        ImGui::SetNextItemWidth(150.0f);
        ImGui::BeginDisabled();
        ImGui::Combo("Classificação do aço ativo", &dados.tipo_aco_ativo, tipos_aco_ativo, 4);
        ImGui::EndDisabled();

        static const char* tipos_protensao[] = {
            "Pre-tracionada",
            "Pos-tracionada aderente",
            "Pos-tracionada não aderente",
            "Barras de aço"
        };

        ImGui::SetNextItemWidth(240.0f);
        ImGui::Combo("Tipo de protensão", &dados.tipo_protensao, tipos_protensao, 4);

        static const char* tipos_cabo_e_protensao[] = {
            "Cabo e concreto sem bainha",
            "Barras/fios com mossas e bainha metálica",
            "Fios lisos/cordoalhas e bainha metálica",
            "Fios lisos/cordoalhas e bainha lubrificada",
            "Cordoalha e bainha polipropileno lubrificada"
        };

        ImGui::SetNextItemWidth(360.0f);
        ImGui::Combo("Relação cabo/protensão", &dados.tipo_cabo_e_protensao, tipos_cabo_e_protensao, 5);

        static const char* niveis_protensao[] = {
            "Nivel 1 - protensão parcial",
            "Nivel 2 - protensão limitada",
            "Nivel 3 - protensão completa"
        };

        ImGui::SetNextItemWidth(260.0f);
        ImGui::Combo("Nivel de protensão", &dados.nivel_protensao, niveis_protensao, 3);

        ImGui::BeginDisabled();
        ImGui::SetNextItemWidth(100.0f);
        ImGui::InputDouble("fptk do produto (MPa)", &dados.fptk, 0.0, 0.0, "%.2f");
        ImGui::SetNextItemWidth(90.0f);
        ImGui::InputDouble("Diâmetro nominal (mm)", &dados.diametro_cordoalha_mm, 0.0, 0.0, "%.1f");
        ImGui::SetNextItemWidth(80.0f);
        ImGui::InputDouble("Área cordoalha (cm2)", &dados.area_cordoalha_cm2, 0.0, 0.0, "%.2f");
        ImGui::EndDisabled();

        ImGui::SetNextItemWidth(80.0f);
        ImGui::InputDouble("Fator fpyk", &dados.fator_fpyk, 0.0, 0.0, "%.3f");
        ImGui::SetNextItemWidth(90.0f);
        ImGui::InputDouble("E aço ativo (GPa)", &dados.modulo_E_aco_prot, 0.0, 0.0, "%.2f");
        ImGui::SetNextItemWidth(70.0f);
        ImGui::InputInt("Número de cordoalhas", &dados.numero_cordoalhas);

        DadosProdutoCordoalha produto_selecionado = ObterDadosProdutoCordoalha(dados.produto_cordoalha);
        ImGui::Text(
            "Catálogo: ruptura mínima %.0f kN | carga a 1%% %.0f kN | alongamento %.1f%%",
            produto_selecionado.carga_minima_ruptura_kN,
            produto_selecionado.carga_minima_1_porcento_kN,
            produto_selecionado.alongamento_ruptura_percento
        );
    }

    if (ImGui::CollapsingHeader("Seção", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::SetNextItemWidth(90.0f);
        ImGui::InputDouble("Altura seção (cm)", &dados.altura_secao_cm, 0.0, 0.0, "%.2f");
        ImGui::SetNextItemWidth(90.0f);
        ImGui::InputDouble("Largura seção (cm)", &dados.largura_secao_cm, 0.0, 0.0, "%.2f");
    }

    if (ImGui::CollapsingHeader("Condições ambientais")) {
        ImGui::SetNextItemWidth(90.0f);
        ImGui::InputDouble("Temperatura (C)", &dados.temperatura, 0.0, 0.0, "%.2f");
        ImGui::SetNextItemWidth(90.0f);
        ImGui::InputDouble("Umidade (%)", &dados.umidade, 0.0, 0.0, "%.2f");
    }

    if (ImGui::CollapsingHeader("Cabo", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Checkbox("Usar traçado de Cabral (2024)", &dados.usar_tracado_pedro);
        ImGui::SetNextItemWidth(90.0f);
        ImGui::InputDouble("Altura no apoio (cm)", &dados.altura_do_cabo_no_apoio_cm, 0.0, 0.0, "%.2f");
        ImGui::SetNextItemWidth(90.0f);
        ImGui::InputDouble("Excentricidade máxima do Cabo (cm)", &dados.excentricidade_max_cabo_cm, 0.0, 0.0, "%.2f");

        if (dados.usar_tracado_pedro) {
            dados.numero_pontos = 21;
            ImGui::Text("Número de pontos fixado em 21 para o traçado de Cabral (2024).");
        }
        else {
            ImGui::SetNextItemWidth(70.0f);
            ImGui::InputInt("Número de pontos", &dados.numero_pontos);
        }

        double coeficiente_atrito_mostrado = CoeficienteAtritoCurvaturaPorTipoCabo(static_cast<TipoCaboeProtensao>(dados.tipo_cabo_e_protensao));
        ImGui::Text("Coeficiente de atrito adotado = %.2f", coeficiente_atrito_mostrado);
        ImGui::SetNextItemWidth(90.0f);
        ImGui::InputDouble("Escorregamento ancoragem (mm)", &dados.escorregamento_ancoragem_mm, 0.0, 0.0, "%.2f");
    }

    if (ImGui::CollapsingHeader("Solicitações", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::SetNextItemWidth(90.0f);
        ImGui::InputDouble("Vão (m)", &dados.vao, 0.0, 0.0, "%.2f");
        ImGui::SetNextItemWidth(100.0f);
        ImGui::InputDouble("NSk (kN)", &dados.NSd_kN, 0.0, 0.0, "%.2f");
        ImGui::SetNextItemWidth(90.0f);
        ImGui::InputDouble("qk (kN/m)", &dados.qSd_kN_por_m, 0.0, 0.0, "%.2f");

        static const char* tipos_acao_variavel[] = {
            "Residências/hospedagens/acesso restrito",
            "Comércio/escritórios/lojas/públicos",
            "Bibliotecas/arquivos/oficinas/garagens",
            "Vento",
            "Temperatura"
        };

        ImGui::SetNextItemWidth(320.0f);
        ImGui::Combo("Tipo de ação variavel", &dados.tipo_acao_variavel, tipos_acao_variavel, 5);

        TipoCombinacaoServico combinacao_interface_els_f = CombinacaoELSFPorNivelProtensao(static_cast<NivelProtensao>(dados.nivel_protensao));
        TipoCombinacaoServico combinacao_interface_els_d = CombinacaoELSDPorNivelProtensao(static_cast<NivelProtensao>(dados.nivel_protensao));
        double fator_interface_els_f = FatorCombinacaoServico(combinacao_interface_els_f, static_cast<TipoAcaoVariavel>(dados.tipo_acao_variavel));
        double fator_interface_els_d = FatorCombinacaoServico(combinacao_interface_els_d, static_cast<TipoAcaoVariavel>(dados.tipo_acao_variavel));

        ImGui::Text("ELS-F: combinação %s | psi = %.2f", NomeCombinacaoServico(combinacao_interface_els_f).c_str(), fator_interface_els_f);
        ImGui::Text("ELS-D: combinação %s | psi = %.2f", NomeCombinacaoServico(combinacao_interface_els_d).c_str(), fator_interface_els_d);

        ImGui::SetNextItemWidth(70.0f);
        ImGui::InputInt("Dias até ato protensão", &dados.dias_ato_protensao);
    }

}

void DesenharPaginaResultados(VigaConcretoProtendido& viga, bool& viga_dimensionada)
{
    if (!viga_dimensionada) {
        return;
    }

    if (ImGui::Button("Exportar viga atual CSV para Excel")) {
        ExportarResultadosVigaCSV(
            viga,
            "resultado_viga_interface.csv"
        );
    }

    ImGui::SameLine();

    if (ImGui::Button("Exportar viga atual TXT")) {
        std::vector<VigaConcretoProtendido> vigas;
        vigas.push_back(viga);
        ExportarResultadosVigas(vigas, "resultado_viga_interface.txt");
    }

    ImGui::Separator();

    ImGui::Text("Resultado atual");
    ImGui::Text("ELS geral: %s", viga.verificacoes_els_aprovadas ? "APROVADO" : "REPROVADO");
    ImGui::Text("Ato: %s", viga.ato_protensao_aprovado ? "APROVADO" : "REPROVADO");
    ImGui::Text("ELS-F: %s", viga.verificacoes_els_f_aprovadas ? "APROVADO" : "REPROVADO");
    ImGui::Text("ELS-D: %s", viga.verificacoes_els_d_aprovadas ? "APROVADO" : "REPROVADO");
    ImGui::Text("ELU: %s", viga.verificacoes_elu_aprovadas ? "CALCULADO" : "ERRO NO CALCULO");
    ImGui::Text("Nivel de protensão = %s", NomeNivelProtensao(viga.cabo.armadura_ativa.nivel_protensao).c_str());
    ImGui::Text("ELS-F usa combinação %s, psi = %.2f", NomeCombinacaoServico(viga.combinacao_els_f).c_str(), viga.fator_combinacao_els_f);
    ImGui::Text("ELS-D usa combinação %s, psi = %.2f", NomeCombinacaoServico(viga.combinacao_els_d).c_str(), viga.fator_combinacao_els_d);
    ImGui::Text("Coeficiente de atrito adotado = %.2f", viga.cabo.coeficiente_atrito_curvatura);
    ImGui::Text("Msd crítico ELU = %.2f kN.m", viga.maior_momento_solicitante_elu_kNm);
    ImGui::Text("As máxima ELU = %.2f cm2", viga.maior_As_elu * 1.0e4);
    ImGui::Text("x/d crítico = %.4f", viga.x_sobre_d);
    ImGui::Text("Perda Total de tensão = %.2f MPa", viga.maior_perda_progressiva_total_MPa);
    ImGui::Text("Perdas Totais de tensão em porcentagem = %.2f %%", viga.maior_percentual_perda_total * 100.0);


    int indice;
    double forca_meio_vao;

    if((int)viga.cabo.pontos.size() % 2 == 0){
        indice = (int)viga.cabo.pontos.size() / 2;
        forca_meio_vao = (viga.cabo.pontos[indice].forca_apos_perda_progressiva_kN + viga.cabo.pontos[indice - 1].forca_apos_perda_progressiva_kN) / 2.0;
    }
    else{
        indice = ((int)viga.cabo.pontos.size() - 1) / 2 + 1;
        forca_meio_vao = viga.cabo.pontos[indice].forca_apos_perda_progressiva_kN;
    }

    int indice_meio_do_vao;
    double perda_meio_vao;

    if((int)viga.cabo.trechos.size() % 2 == 0){
        indice_meio_do_vao = (int)viga.cabo.trechos.size() / 2 - 1;
        perda_meio_vao = viga.cabo.trechos[indice_meio_do_vao].perda_forca_atrito;
    }
    else{
        indice_meio_do_vao = ((int)viga.cabo.trechos.size() - 1) / 2;
        perda_meio_vao = (viga.cabo.trechos[indice_meio_do_vao].perda_forca_atrito + viga.cabo.trechos[indice_meio_do_vao - 1].perda_forca_atrito) / 2.0;
    }

    double perda_percentual = perda_meio_vao / viga.cabo.armadura_ativa.forca_inicial * 100.0;

    ImGui::Separator();
    ImGui::Text("Dados do cabo");
    ImGui::Text("Área total de armadura ativa = %.2f cm2", viga.cabo.armadura_ativa.area_total * 1.0e4);
    ImGui::Text("Tensão inicial = %.2f MPa", viga.cabo.armadura_ativa.tensao_inicial / 1.0e6);
    ImGui::Text("Forca inicial = %.2f kN", viga.cabo.armadura_ativa.forca_inicial / 1000.0);
    ImGui::Text("Forca final = %.2f kN", viga.cabo.armadura_ativa.forca_final / 1000.0);
    ImGui::Text("Forca do meio do vão = %.2f kN", forca_meio_vao);

    ImGui::Separator();
    ImGui::Text("Encunhamento");
    ImGui::Text("Perda na ancoragem = %.3f MPa", viga.cabo.perda_na_ancoragem_encunhamento_MPa);
    ImGui::Text("Perda no meio do vão = %.2f kN", perda_meio_vao / 1000.0);
    ImGui::Text("Perda percentual no meio do vão = %.2f %%", perda_percentual);
}


struct InterfaceAplicacao
{
    DadosEntradaInterface dados_entrada;
    VigaConcretoProtendido viga;
    bool viga_dimensionada;
    int pagina_atual;
    bool forcar_selecao_pagina;
    Graficos graficos;

    InterfaceAplicacao()
        : dados_entrada(CriarDadosEntradaPadraoOriginal()),
          viga(CriarVigaPadraoOriginal())
    {
        viga_dimensionada = false;
        pagina_atual = 1;
        forcar_selecao_pagina = false;
    }

    void inicializarJanela()
    {
        SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);

        InitWindow(
            (int)tamanho_paginas_geral.x + 40,
            (int)tamanho_paginas_geral.y + 60,
            "Viga de concreto protendido - interface"
        );

        SetExitKey(KEY_NULL);
        SetTargetFPS(244);
    }

    void carregarFonteComSimbolos()
    {
        ImGuiIO& io = ImGui::GetIO();

        static const ImWchar intervalos_unicode[] = {
            0x0020, 0x00FF, // caracteres básicos e acentuação
            0x0370, 0x03FF, // alfabeto grego: α, β, γ, σ, Σ...
            0x2000, 0x206F, // pontuação geral
            0x2200, 0x22FF, // operadores matemáticos: ∞, Δ relacionados etc.
            0
        };

        const char* fontes_sistema[] = {
            "C:/Windows/Fonts/segoeui.ttf",
            "C:/Windows/Fonts/arial.ttf",
            "C:/Windows/Fonts/tahoma.ttf",
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
            "/System/Library/Fonts/Supplemental/Arial.ttf"
        };

        ImFontConfig configuracao_fonte;
        configuracao_fonte.OversampleH = 2;
        configuracao_fonte.OversampleV = 2;
        configuracao_fonte.PixelSnapH = true;

        for (int i = 0; i < 5; i++) {
            if (!FileExists(fontes_sistema[i])) {
                continue;
            }

            ImFont* fonte = io.Fonts->AddFontFromFileTTF(
                fontes_sistema[i],
                15.0f,
                &configuracao_fonte,
                intervalos_unicode
            );

            if (fonte != 0) {
                io.FontDefault = fonte;
                break;
            }
        }
    }

    void inicializarImGui()
    {
        // Inicialização personalizada para adicionar a fonte antes de o
        // atlas de fontes do rlImGui ser finalizado.
        rlImGuiBeginInitImGui();
        ImGui::StyleColorsLight();

        ImGui::GetIO().IniFilename = 0;
        carregarFonteComSimbolos();

        ImGui::GetStyle().AntiAliasedLines = true;
        ImGui::GetStyle().AntiAliasedLinesUseTex = false;
        ImGui::GetStyle().AntiAliasedFill = true;

        rlImGuiEndInitImGui();
        ImPlot::CreateContext();
    }

    void finalizarAplicacao()
    {
        ImPlot::DestroyContext();
        rlImGuiShutdown();
        CloseWindow();
    }

    void processarComandosTeclado()
    {
        bool ctrl_pressionado =
            IsKeyDown(KEY_LEFT_CONTROL) ||
            IsKeyDown(KEY_RIGHT_CONTROL);

        bool shift_pressionado =
            IsKeyDown(KEY_LEFT_SHIFT) ||
            IsKeyDown(KEY_RIGHT_SHIFT);

        if (ctrl_pressionado && shift_pressionado && IsKeyPressed(KEY_B)) {
            viga = GerarVigaAleatoria();
            PreencherDadosEntradaComViga(dados_entrada, viga);
            viga.dimensionar();
            viga_dimensionada = true;
        }

        if (ctrl_pressionado && shift_pressionado && IsKeyPressed(KEY_A)) {
            TestarVigasAleatorias();
        }
    }

    ImGuiTabItemFlags flagPagina(int pagina)
    {
        return forcar_selecao_pagina && pagina_atual == pagina
            ? ImGuiTabItemFlags_SetSelected
            : ImGuiTabItemFlags_None;
    }

    void desenharPaginaEntradaPrincipal()
    {
        if (ImGui::BeginTabItem(
            "Entrada e geometria",
            0,
            flagPagina(1)
        )) {
            pagina_atual = 1;

            ImGui::BeginChild(
                "ConteudoPagina1",
                ImVec2(0, -45),
                true,
                ImGuiWindowFlags_AlwaysVerticalScrollbar
            );

            DesenharPaginaEntrada(
                dados_entrada,
                viga,
                viga_dimensionada
            );

            if (viga_dimensionada) {
                ImGui::Spacing();
                graficos.desenharEsquemasDaViga(viga);
            }
            else {
                ImGui::Separator();
                ImGui::Text(
                    "Dimensione a viga para visualizar o esquema estrutural, a seção transversal e o traçado do cabo."
                );
            }

            ImGui::EndChild();
            ImGui::EndTabItem();
        }
    }

    void desenharPaginaGrafica(
        const char* titulo_aba,
        const char* id_conteudo,
        int numero_pagina,
        const char* mensagem
    )
    {
        if (ImGui::BeginTabItem(
            titulo_aba,
            0,
            flagPagina(numero_pagina)
        )) {
            pagina_atual = numero_pagina;

            ImGui::BeginChild(
                id_conteudo,
                ImVec2(0, -45),
                true,
                ImGuiWindowFlags_AlwaysVerticalScrollbar
            );

            if (viga_dimensionada) {
                graficos.desenharPagina(viga, numero_pagina);
            }
            else {
                ImGui::TextUnformatted(mensagem);
            }

            ImGui::EndChild();
            ImGui::EndTabItem();
        }
    }

    void desenharPaginaResultadosPrincipal()
    {
        if (ImGui::BeginTabItem(
            "Resumo",
            0,
            flagPagina(9)
        )) {
            pagina_atual = 9;

            ImGui::BeginChild(
                "ConteudoPagina9",
                ImVec2(0, -45),
                true,
                ImGuiWindowFlags_AlwaysVerticalScrollbar
            );

            if (viga_dimensionada) {
                DesenharPaginaResultados(viga, viga_dimensionada);
            }
            else {
                ImGui::Text("Dimensione a viga para visualizar o resumo.");
            }

            ImGui::EndChild();
            ImGui::EndTabItem();
        }
    }

    void desenharAbas()
    {
        if (!ImGui::BeginTabBar(
            "PaginasPrincipais",
            ImGuiTabBarFlags_None
        )) {
            return;
        }

        desenharPaginaEntradaPrincipal();

        desenharPaginaGrafica(
            "Atrito e encunhamento",
            "ConteudoPagina2",
            2,
            "Dimensione a viga para visualizar as perdas por atrito e encunhamento."
        );

        desenharPaginaGrafica(
            "Encurtamento elástico",
            "ConteudoPagina3",
            3,
            "Dimensione a viga para visualizar a perda por encurtamento elástico."
        );

        desenharPaginaGrafica(
            "Ato da protensão",
            "ConteudoPagina4",
            4,
            "Dimensione a viga para visualizar o ato da protensão."
        );

        desenharPaginaGrafica(
            "Perdas progressivas",
            "ConteudoPagina5",
            5,
            "Dimensione a viga para visualizar as perdas progressivas."
        );

        desenharPaginaGrafica(
            "ELS-F",
            "ConteudoPagina6",
            6,
            "Dimensione a viga para visualizar o ELS-F."
        );

        desenharPaginaGrafica(
            "ELS-D",
            "ConteudoPagina7",
            7,
            "Dimensione a viga para visualizar o ELS-D."
        );

        desenharPaginaGrafica(
            "ELU",
            "ConteudoPagina8",
            8,
            "Dimensione a viga para visualizar o ELU."
        );

        desenharPaginaResultadosPrincipal();

        ImGui::EndTabBar();
        forcar_selecao_pagina = false;
    }

    void desenharBotaoAnterior()
    {
        if (pagina_atual > 1) {
            if (ImGui::Button("< Anterior", ImVec2(120, 30))) {
                pagina_atual--;
                forcar_selecao_pagina = true;
            }
        }
        else {
            ImGui::BeginDisabled();
            ImGui::Button("< Anterior", ImVec2(120, 30));
            ImGui::EndDisabled();
        }
    }

    void posicionarBotaoDireita()
    {
        ImGui::SameLine();

        float largura_disponivel = ImGui::GetContentRegionAvail().x;
        float largura_botao =
            pagina_atual == 1
            ? 240.0f
            : 120.0f;

        if (largura_disponivel > largura_botao) {
            ImGui::SetCursorPosX(
                ImGui::GetCursorPosX()
                + largura_disponivel
                - largura_botao
            );
        }
    }

    void desenharBotaoDireita()
    {
        if (pagina_atual == 1) {
            if (ImGui::Button(
                "Dimensionar dados da interface",
                ImVec2(240, 30)
            )) {
                viga = CriarVigaPelosDadosDaInterface(dados_entrada);
                viga.dimensionar();
                viga_dimensionada = true;
                pagina_atual = 2;
                forcar_selecao_pagina = true;
            }
        }
        else if (pagina_atual < 9) {
            if (ImGui::Button("Próxima >", ImVec2(120, 30))) {
                pagina_atual++;
                forcar_selecao_pagina = true;
            }
        }
        else {
            ImGui::BeginDisabled();
            ImGui::Button("Próxima >", ImVec2(120, 30));
            ImGui::EndDisabled();
        }
    }

    void desenharNavegacao()
    {
        ImGui::Separator();
        desenharBotaoAnterior();
        posicionarBotaoDireita();
        desenharBotaoDireita();
    }

    void desenharJanelaPrincipal()
    {
        ImGui::SetNextWindowPos(
            ImVec2(20, 20),
            ImGuiCond_FirstUseEver
        );

        ImGui::SetNextWindowSize(
            tamanho_paginas_geral,
            ImGuiCond_FirstUseEver
        );

        ImGuiWindowFlags flags_janela =
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoSavedSettings;

        ImGui::Begin(
            "Viga de concreto protendido - interface",
            0,
            flags_janela
        );

        desenharAbas();
        desenharNavegacao();

        ImGui::End();
    }

    void desenharFrame()
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        processarComandosTeclado();

        rlImGuiBegin();
        desenharJanelaPrincipal();
        rlImGuiEnd();

        EndDrawing();
    }

    void executar()
    {
        std::cout << "Seguindo a NBR 6118." << std::endl;

        inicializarJanela();
        inicializarImGui();

        while (!WindowShouldClose()) {
            desenharFrame();
        }

        finalizarAplicacao();
    }
};

int main()
{
    InterfaceAplicacao aplicacao;
    aplicacao.executar();

    return 0;
}
