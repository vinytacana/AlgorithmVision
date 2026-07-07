package main.ui;

import main.controle.ControladorSimulacao;

import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JSlider;
import javax.swing.JTextArea;
import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.FlowLayout;

/**
 * Painel lateral com os controles da simulacao.
 *
 * HERANCA: estende {@link JPanel}. Depende apenas da abstracao
 * {@link ControladorSimulacao} (inversao de dependencia) - nao conhece JNI,
 * OpenGL nem qualquer detalhe do motor nativo.
 */
public class PainelControles extends JPanel {

    private static final long serialVersionUID = 1L;

    /** Mapeamento logaritmico do slider de velocidade: 0..100 -> 0.001s..0.5s. */
    private static final float TICK_MINIMO = 0.001f;
    private static final float TICK_MAXIMO = 0.5f;

    private final transient ControladorSimulacao controlador;
    private final JButton botaoIniciarPausar;
    private final JLabel rotuloStatus;
    private final JLabel rotuloComparacoes;
    private final JLabel rotuloAcessos;
    private final JTextArea areaDescricao;

    public PainelControles(ControladorSimulacao controlador) {
        this.controlador = controlador;

        setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
        setBorder(BorderFactory.createEmptyBorder(10, 12, 10, 12));

        // ----- Algoritmo e dados -----
        final String[] algoritmos = new String[controlador.quantidadeAlgoritmos()];
        for (int i = 0; i < algoritmos.length; i++) {
            algoritmos[i] = controlador.nomeAlgoritmo(i);
        }
        final JComboBox<String> comboAlgoritmo = new JComboBox<>(algoritmos);

        // ATENCAO: a ordem destes itens e contrato com os enums nativos em
        // include/SortTypes.h (ArrayDistribution e RenderMode) — o indice
        // selecionado viaja pelo JNI. Ao alterar, atualize os dois lados.
        final JComboBox<String> comboDistribuicao = new JComboBox<>(new String[] {
                "Aleatório", "Reverso", "Quase Ordenado", "Poucos Únicos"
        });
        final JComboBox<String> comboVisualizacao = new JComboBox<>(new String[] {
                "Barras", "Dot Plot", "Circular"
        });

        final JSlider sliderTamanho = new JSlider(10, 1000, 100);
        final JLabel valorTamanho = new JLabel("100");
        final JSlider sliderVelocidade = new JSlider(0, 100, tickParaSlider(0.03f));
        final JLabel valorVelocidade = new JLabel(formatarTick(0.03f));

        // ----- Descricao pedagogica -----
        areaDescricao = new JTextArea(6, 24);
        areaDescricao.setLineWrap(true);
        areaDescricao.setWrapStyleWord(true);
        areaDescricao.setEditable(false);
        areaDescricao.setOpaque(false);
        areaDescricao.setText(controlador.descricaoAlgoritmo(0));

        // ----- Controles e metricas -----
        botaoIniciarPausar = new JButton("Iniciar");
        final JButton botaoResetar = new JButton("Resetar");
        rotuloStatus = new JLabel("Status: Pronto");
        rotuloComparacoes = new JLabel("Comparações: 0");
        rotuloAcessos = new JLabel("Acessos: 0");

        // ----- Listeners: cada acao delega para a ABSTRACAO -----
        comboAlgoritmo.addActionListener(e -> {
            final int indice = comboAlgoritmo.getSelectedIndex();
            controlador.selecionarAlgoritmo(indice);
            areaDescricao.setText(controlador.descricaoAlgoritmo(indice));
        });
        comboDistribuicao.addActionListener(e ->
                controlador.selecionarDistribuicao(comboDistribuicao.getSelectedIndex()));
        comboVisualizacao.addActionListener(e ->
                controlador.selecionarVisualizacao(comboVisualizacao.getSelectedIndex()));
        sliderTamanho.addChangeListener(e -> {
            valorTamanho.setText(String.valueOf(sliderTamanho.getValue()));
            if (!sliderTamanho.getValueIsAdjusting()) {
                controlador.definirTamanho(sliderTamanho.getValue());
            }
        });
        sliderVelocidade.addChangeListener(e -> {
            final float tick = sliderParaTick(sliderVelocidade.getValue());
            valorVelocidade.setText(formatarTick(tick));
            controlador.definirVelocidade(tick);
        });
        botaoIniciarPausar.addActionListener(e -> controlador.iniciarOuPausar());
        botaoResetar.addActionListener(e -> controlador.reiniciar());

        // ----- Montagem -----
        add(rotulado("Algoritmo", comboAlgoritmo));
        add(rotulado("Distribuição", comboDistribuicao));
        add(rotulado("Visualização", comboVisualizacao));
        add(rotuladoComValor("Tamanho (N)", sliderTamanho, valorTamanho));
        add(rotuladoComValor("Velocidade (s/passo)", sliderVelocidade, valorVelocidade));

        final JPanel botoes = new JPanel(new FlowLayout(FlowLayout.LEFT, 8, 4));
        botoes.add(botaoIniciarPausar);
        botoes.add(botaoResetar);
        add(botoes);

        add(alinhadoEsquerda(rotuloStatus));
        add(alinhadoEsquerda(rotuloComparacoes));
        add(alinhadoEsquerda(rotuloAcessos));

        final JPanel painelInfo = new JPanel(new BorderLayout());
        painelInfo.setBorder(BorderFactory.createTitledBorder("Informação Pedagógica"));
        painelInfo.add(areaDescricao, BorderLayout.CENTER);
        add(painelInfo);

        setPreferredSize(new Dimension(340, getPreferredSize().height));
    }

    /** Atualiza rotulos de status/metricas. Chamado pelo laco de animacao. */
    public void atualizar() {
        rotuloStatus.setText("Status: " + controlador.obterStatus());
        rotuloComparacoes.setText("Comparações: " + controlador.obterComparacoes());
        rotuloAcessos.setText("Acessos: " + controlador.obterAcessos());
        botaoIniciarPausar.setText(controlador.estaExecutando() ? "Pausar" : "Iniciar");
    }

    // ----- Helpers de layout -----

    private static JPanel rotulado(String titulo, JComboBox<String> combo) {
        final JPanel painel = new JPanel(new BorderLayout(8, 0));
        painel.add(new JLabel(titulo), BorderLayout.WEST);
        painel.add(combo, BorderLayout.CENTER);
        painel.setBorder(BorderFactory.createEmptyBorder(2, 0, 2, 0));
        return painel;
    }

    private static JPanel rotuladoComValor(String titulo, JSlider slider, JLabel valor) {
        final JPanel painel = new JPanel(new BorderLayout(8, 0));
        painel.add(new JLabel(titulo), BorderLayout.WEST);
        painel.add(slider, BorderLayout.CENTER);
        painel.add(valor, BorderLayout.EAST);
        painel.setBorder(BorderFactory.createEmptyBorder(2, 0, 2, 0));
        return painel;
    }

    private static JPanel alinhadoEsquerda(JLabel rotulo) {
        final JPanel painel = new JPanel(new FlowLayout(FlowLayout.LEFT, 0, 2));
        painel.add(rotulo);
        return painel;
    }

    // ----- Conversao slider <-> segundos por passo (escala logaritmica) -----

    private static float sliderParaTick(int valor) {
        final double razao = valor / 100.0;
        return (float) (TICK_MINIMO * Math.pow(TICK_MAXIMO / TICK_MINIMO, razao));
    }

    private static int tickParaSlider(float tick) {
        final double razao = Math.log(tick / TICK_MINIMO) / Math.log(TICK_MAXIMO / TICK_MINIMO);
        return (int) Math.round(razao * 100.0);
    }

    private static String formatarTick(float tick) {
        return String.format("%.3f", tick);
    }
}
