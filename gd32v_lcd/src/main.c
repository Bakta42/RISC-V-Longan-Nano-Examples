#include "lcd/lcd.h"
#include <string.h>
#include <trig.h>
//#include <math.h>

void init_uart0(void)
{
	/* enable GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    /* enable USART clock */
    rcu_periph_clock_enable(RCU_USART0);

    /* connect port to USARTx_Tx */
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
    /* connect port to USARTx_Rx */
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_10);

	/* USART configure */
    usart_deinit(USART0);
    usart_baudrate_set(USART0, 115200U);
    usart_word_length_set(USART0, USART_WL_8BIT);
    usart_stop_bit_set(USART0, USART_STB_1BIT);
    usart_parity_config(USART0, USART_PM_NONE);
    usart_hardware_flow_rts_config(USART0, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(USART0, USART_CTS_DISABLE);
    usart_receive_config(USART0, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
    usart_enable(USART0);

    usart_interrupt_enable(USART0, USART_INT_RBNE);
}


// Code below written by Marijn Stollenga:

void draw_mandel() {
  int const width = 160;
  int const height = 80;
  LCD_Address_Set(0, 0, width - 1, height - 1);      //设置光标位置

  for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x)
    {
      float ca = 0.002 * (x - 80) / 80. - 0.7463;
      float cb = 0.002 * (y - 40) / 80. + 0.1102;
      float a = ca;
      float b = cb;
      int n = 0;
      const int factor = 16;
      for (; n < 65536 / factor; ++n) {
        float ta = a * a - b * b;
        if (ta > 2.0)
          break;
        b = cb + 2 * a * b;
        a = ca + ta;

      }
      LCD_WR_DATA(n * 3);
    }
}

uint16_t get_color(uint8_t r, uint8_t g, uint8_t b) {
  return ((uint16_t)(r >> 3) << 11) + ((uint16_t)(g >> 2) << 5) + ((uint16_t)(b >> 3));
}

void draw_angle() {
  uint16_t color = 0;
  int const width = 160;
  int const height = 80;

  while (1) {
    LCD_Address_Set(0, 0, width - 1, height - 1);      //设置光标位置
    for (int y = 0; y < height; ++y)
      for (int x = 0; x < width; ++x)
        LCD_WR_DATA(get_color(y * 256 / height, color, x * 256 / width));
    // delay_1ms(1);
  }
}

uint16_t RGB(const uint8_t red, const uint8_t green, uint8_t blue) {
  return ((red & 0x1f) << 11) | ((green & 0x3f) << 5) | (blue & 0x1f);
}

const float PI = 3.1415926535f;
const float PIh = PI / 2;
const float sincos_tbl[1024] = {0.0f, 0.001f};
const int sincos_tlb_len = sizeof(sincos_tbl) / sizeof(float);
const int sincos_tbli[1025] = {0, 25735, 51471, 77207, 102943, 128678, 154413, 180148, 205882, 231615, 257349, 283081, 308813, 334544, 360275, 386004, 411733, 437461, 463187, 488913, 514637, 540360, 566082, 591803, 617522, 643240, 668956, 694671, 720384, 746095,
  771805, 797513, 823218, 848922, 874624, 900324, 926022, 951718, 977411, 1003102, 1028791, 1054477, 1080161, 1105842, 1131521, 1157197, 1182870, 1208540, 1234208, 1259873, 1285535, 1311193, 1336849, 1362501, 1388151, 1413797, 1439440, 1465079, 1490715, 1516347,
  1541976, 1567601, 1593223, 1618840, 1644454, 1670064, 1695670, 1721273, 1746871, 1772465, 1798054, 1823640, 1849221, 1874798, 1900371, 1925939, 1951502, 1977061, 2002615, 2028165, 2053710, 2079250, 2104785, 2130315, 2155840, 2181360, 2206875, 2232385, 2257889,
  2283388, 2308882, 2334370, 2359853, 2385330, 2410802, 2436268, 2461728, 2487183, 2512631, 2538074, 2563511, 2588942, 2614366, 2639785, 2665197, 2690603, 2716002, 2741396, 2766783, 2792163, 2817537, 2842904, 2868264, 2893618, 2918965, 2944304, 2969638, 2994964,
  3020283, 3045595, 3070899, 3096197, 3121487, 3146770, 3172045, 3197313, 3222574, 3247827, 3273072, 3298310, 3323539, 3348761, 3373975, 3399182, 3424380, 3449570, 3474752, 3499926, 3525091, 3550248, 3575397, 3600538, 3625670, 3650793, 3675908, 3701015, 3726112,
  3751201, 3776281, 3801352, 3826414, 3851467, 3876511, 3901546, 3926572, 3951589, 3976596, 4001594, 4026582, 4051561, 4076530, 4101490, 4126441, 4151381, 4176312, 4201233, 4226144, 4251045, 4275936, 4300817, 4325688, 4350548, 4375399, 4400239, 4425069, 4449888,
  4474697, 4499495, 4524283, 4549060, 4573827, 4598583, 4623327, 4648061, 4672784, 4697497, 4722198, 4746887, 4771566, 4796234, 4820890, 4845535, 4870168, 4894790, 4919401, 4944000, 4968587, 4993163, 5017726, 5042278, 5066819, 5091347, 5115863, 5140367, 5164859,
  5189339, 5213807, 5238263, 5262706, 5287137, 5311555, 5335961, 5360354, 5384735, 5409103, 5433458, 5457801, 5482130, 5506447, 5530751, 5555042, 5579319, 5603584, 5627835, 5652073, 5676298, 5700510, 5724708, 5748892, 5773063, 5797221, 5821365, 5845495, 5869611,
  5893714, 5917803, 5941877, 5965938, 5989985, 6014018, 6038036, 6062041, 6086031, 6110006, 6133968, 6157915, 6181847, 6205765, 6229668, 6253557, 6277431, 6301290, 6325134, 6348964, 6372778, 6396578, 6420362, 6444131, 6467886, 6491625, 6515348, 6539057, 6562750,
  6586427, 6610089, 6633736, 6657366, 6680982, 6704581, 6728165, 6751733, 6775285, 6798821, 6822341, 6845845, 6869332, 6892804, 6916260, 6939699, 6963122, 6986528, 7009918, 7033292, 7056649, 7079989, 7103313, 7126620, 7149910, 7173184, 7196440, 7219680, 7242902,
  7266108, 7289297, 7312468, 7335622, 7358759, 7381878, 7404981, 7428065, 7451133, 7474182, 7497215, 7520229, 7543226, 7566205, 7589166, 7612110, 7635035, 7657943, 7680832, 7703704, 7726557, 7749392, 7772209, 7795008, 7817788, 7840550, 7863293, 7886018, 7908724,
  7931412, 7954081, 7976732, 7999363, 8021976, 8044570, 8067145, 8089701, 8112238, 8134756, 8157254, 8179734, 8202194, 8224635, 8247057, 8269459, 8291842, 8314205, 8336549, 8358873, 8381177, 8403462, 8425727, 8447972, 8470197, 8492402, 8514587, 8536753, 8558898,
  8581023, 8603128, 8625212, 8647277, 8669321, 8691344, 8713347, 8735330, 8757292, 8779233, 8801154, 8823054, 8844933, 8866792, 8888629, 8910446, 8932242, 8954016, 8975770, 8997503, 9019214, 9040904, 9062573, 9084221, 9105847, 9127451, 9149035, 9170596, 9192136,
  9213655, 9235152, 9256627, 9278080, 9299512, 9320921, 9342309, 9363675, 9385018, 9406340, 9427639, 9448917, 9470172, 9491404, 9512615, 9533803, 9554968, 9576111, 9597232, 9618330, 9639405, 9660458, 9681488, 9702495, 9723479, 9744441, 9765379, 9786295, 9807187,
  9828057, 9848903, 9869726, 9890526, 9911303, 9932056, 9952786, 9973492, 9994175, 10014835, 10035471, 10056083, 10076672, 10097237, 10117778, 10138295, 10158789, 10179259, 10199704, 10220126, 10240524, 10260897, 10281247, 10301572, 10321873, 10342149, 10362402,
  10382630, 10402833, 10423012, 10443167, 10463297, 10483402, 10503483, 10523539, 10543570, 10563576, 10583558, 10603514, 10623446, 10643353, 10663234, 10683091, 10702922, 10722728, 10742509, 10762265, 10781995, 10801700, 10821380, 10841034, 10860663, 10880266,
  10899843, 10919395, 10938921, 10958421, 10977896, 10997345, 11016768, 11036164, 11055535, 11074880, 11094199, 11113492, 11132759, 11151999, 11171213, 11190401, 11209563, 11228698, 11247807, 11266889, 11285945, 11304974, 11323977, 11342953, 11361902, 11380825,
  11399720, 11418589, 11437432, 11456247, 11475035, 11493796, 11512530, 11531237, 11549917, 11568570, 11587196, 11605794, 11624365, 11642909, 11661425, 11679914, 11698375, 11716809, 11735215, 11753593, 11771944, 11790268, 11808563, 11826831, 11845071, 11863283,
  11881467, 11899623, 11917751, 11935851, 11953923, 11971967, 11989983, 12007970, 12025929, 12043860, 12061763, 12079637, 12097483, 12115300, 12133089, 12150849, 12168581, 12186284, 12203958, 12221604, 12239221, 12256809, 12274368, 12291898, 12309400, 12326872,
  12344316, 12361730, 12379116, 12396472, 12413799, 12431097, 12448365, 12465604, 12482814, 12499995, 12517146, 12534268, 12551360, 12568423, 12585456, 12602459, 12619433, 12636377, 12653292, 12670176, 12687031, 12703856, 12720651, 12737416, 12754151, 12770856,
  12787531, 12804176, 12820791, 12837376, 12853930, 12870455, 12886949, 12903412, 12919846, 12936248, 12952621, 12968963, 12985274, 13001555, 13017805, 13034025, 13050214, 13066372, 13082500, 13098597, 13114663, 13130698, 13146702, 13162675, 13178617, 13194528,
  13210409, 13226258, 13242076, 13257863, 13273618, 13289343, 13305036, 13320698, 13336328, 13351927, 13367495, 13383031, 13398536, 13414009, 13429451, 13444861, 13460239, 13475586, 13490901, 13506184, 13521436, 13536655, 13551843, 13566999, 13582123, 13597215,
  13612275, 13627303, 13642299, 13657263, 13672195, 13687094, 13701961, 13716797, 13731599, 13746370, 13761108, 13775814, 13790487, 13805128, 13819736, 13834312, 13848856, 13863366, 13877845, 13892290, 13906703, 13921083, 13935430, 13949745, 13964026, 13978275,
  13992491, 14006674, 14020824, 14034941, 14049025, 14063076, 14077094, 14091079, 14105031, 14118949, 14132834, 14146686, 14160505, 14174290, 14188042, 14201761, 14215446, 14229098, 14242716, 14256301, 14269852, 14283369, 14296853, 14310304, 14323720, 14337103,
  14350453, 14363768, 14377050, 14390298, 14403512, 14416692, 14429838, 14442950, 14456028, 14469072, 14482083, 14495059, 14508001, 14520909, 14533782, 14546622, 14559427, 14572198, 14584935, 14597637, 14610305, 14622939, 14635538, 14648103, 14660633, 14673129,
  14685590, 14698017, 14710409, 14722766, 14735089, 14747377, 14759631, 14771850, 14784034, 14796183, 14808297, 14820377, 14832422, 14844432, 14856406, 14868346, 14880251, 14892121, 14903956, 14915756, 14927520, 14939250, 14950944, 14962604, 14974228, 14985817,
  14997370, 15008888, 15020371, 15031819, 15043231, 15054608, 15065949, 15077255, 15088526, 15099761, 15110960, 15122124, 15133252, 15144345, 15155402, 15166423, 15177409, 15188359, 15199273, 15210151, 15220994, 15231801, 15242572, 15253307, 15264006, 15274670,
  15285297, 15295889, 15306444, 15316963, 15327447, 15337894, 15348305, 15358680, 15369019, 15379322, 15389589, 15399819, 15410013, 15420171, 15430293, 15440378, 15450427, 15460440, 15470416, 15480356, 15490259, 15500126, 15509956, 15519750, 15529508, 15539229,
  15548913, 15558561, 15568172, 15577747, 15587284, 15596786, 15606250, 15615678, 15625069, 15634423, 15643741, 15653021, 15662265, 15671472, 15680642, 15689775, 15698872, 15707931, 15716953, 15725939, 15734887, 15743798, 15752673, 15761510, 15770310, 15779073,
  15787799, 15796488, 15805139, 15813754, 15822331, 15830871, 15839373, 15847839, 15856267, 15864658, 15873011, 15881327, 15889606, 15897848, 15906052, 15914218, 15922347, 15930439, 15938493, 15946510, 15954489, 15962430, 15970334, 15978201, 15986030, 15993821,
  16001574, 16009290, 16016969, 16024609, 16032212, 16039777, 16047305, 16054794, 16062246, 16069660, 16077036, 16084375, 16091675, 16098938, 16106163, 16113350, 16120499, 16127610, 16134683, 16141718, 16148715, 16155674, 16162596, 16169479, 16176324, 16183131,
  16189900, 16196631, 16203323, 16209978, 16216594, 16223173, 16229713, 16236215, 16242678, 16249104, 16255491, 16261840, 16268151, 16274423, 16280658, 16286853, 16293011, 16299130, 16305211, 16311253, 16317258, 16323223, 16329151, 16335039, 16340890, 16346702,
  16352475, 16358210, 16363907, 16369565, 16375184, 16380765, 16386308, 16391812, 16397277, 16402704, 16408092, 16413441, 16418752, 16424024, 16429258, 16434453, 16439609, 16444727, 16449806, 16454846, 16459847, 16464810, 16469734, 16474619, 16479466, 16484274,
  16489043, 16493773, 16498464, 16503116, 16507730, 16512305, 16516841, 16521338, 16525796, 16530215, 16534596, 16538937, 16543240, 16547503, 16551728, 16555914, 16560061, 16564169, 16568238, 16572267, 16576258, 16580210, 16584123, 16587997, 16591832, 16595627,
  16599384, 16603102, 16606780, 16610420, 16614020, 16617582, 16621104, 16624587, 16628031, 16631436, 16634802, 16638128, 16641416, 16644664, 16647873, 16651043, 16654174, 16657265, 16660318, 16663331, 16666305, 16669240, 16672135, 16674991, 16677808, 16680586,
  16683325, 16686024, 16688684, 16691305, 16693886, 16696429, 16698932, 16701395, 16703819, 16706204, 16708550, 16710857, 16713124, 16715351, 16717540, 16719689, 16721799, 16723869, 16725900, 16727892, 16729844, 16731757, 16733630, 16735465, 16737259, 16739015,
  16740731, 16742408, 16744045, 16745643, 16747201, 16748720, 16750200, 16751640, 16753041, 16754402, 16755724, 16757007, 16758250, 16759453, 16760618, 16761742, 16762828, 16763874, 16764880, 16765847, 16766775, 16767663, 16768511, 16769320, 16770090, 16770820,
  16771511, 16772163, 16772774, 16773347, 16773880, 16774373, 16774827, 16775242, 16775617, 16775952, 16776248, 16776505, 16776722, 16776900, 16777038, 16777137, 16777196, 16777216};
const int sincos_tbli_len = sizeof(sincos_tbli) / sizeof(int);

float m_cos(const float angle) {
  int quadrant = angle / PIh;
  //float altCos = Trig::cos(angle);
  float frac = angle - (quadrant * PIh); // now it is 0..PIh
  if(!(quadrant & 1)) frac = PIh - frac; // go down for
  // lookup the value
  float cosval = sincos_tbl[(int)(sincos_tlb_len * frac)];
  if(quadrant == 1 || quadrant == 2) cosval *= -1;
  return cosval;
}

inline int remapint(const int value, const int inmax, const int outmax) {
  return value * outmax / inmax;
}

/**
 * Calculate cosine based on precalculated values table with scaled input and output as well.
 * @param[in] angle - calculate cosine from this
 * @param[in] inrange - input value scale. Basically 2PI reference
 * @param[in] outrange - output scale - basically times this as one is normal maximum.
 * @returns cosine value of angle in range -outrange..outrange
 * 
 * How this works...
 * cosine has period of two PI and is symmetrical around the y axis
 * This means cos(x) = cos(mod(abs(x), 2*PI)))
 * And also, this 2PI period can be divided into four segments of PI/2, which all produce same
 * cosine, except some are opposite sign and some are opposite direction.
 * Function (on interval from 0 to 2PI) is also y-symmetrical around x = PI.
 * So already corrected input value could be corrected again as:
 * cos(x) = - cos(abs(x - PI))
 * And finally, 
 */ 
int32_t rangedcos_i(const int angle, const int inrange, const int outrange) {
  int result;
  int x = angle;
  int pihalflen = (inrange / 4);  // Inrange is 2PI len
  if (angle < 0) x = -angle;
  int remnantPIhalf = x % pihalflen;
  int quarter2PI = (x / pihalflen) % 4;
  int x_tblIndex = remnantPIhalf * sincos_tbli_len / pihalflen;
  switch (quarter2PI) {
    case 0: result = sincos_tbli[sincos_tbli_len - x_tblIndex - 1]; break;
    case 1: result = -sincos_tbli[x_tblIndex]; break;
    case 2: result = -sincos_tbli[sincos_tbli_len - x_tblIndex - 1]; break;
    case 3: result = sincos_tbli[x_tblIndex]; break;
    default: result = 0; break;
  }
  result = result * outrange / 16777216;
  return result;
}

uint32_t rangedcos_u(const int angle, const int inrange, const int outrange) {
  return rangedcos_i(angle, inrange, outrange / 2) + outrange / 2;
}

uint8_t rangedcos(const int value, const int inrange, const int outrange, const float cosmod) {
  // Calculated cosine
  return (uint8_t)(outrange * (0.5 + 0.5 * mbcos(cosmod + (float)value / inrange)));
}

void draw_1d_ca(int rule) {
  int const width = 160;
  int const height = 80;
  uint8_t life[width * height];
  uint8_t life_line[width];
  uint8_t life_line_next[width];
  uint8_t rules[8];
  // const uint16_t colormap[] = {BLACK, WHITE, BLUE, BRED, GRED, GBLUE, RED, MAGENTA, GREEN, CYAN, YELLOW, BROWN, BRRED, GRAY, DARKBLUE, LIGHTBLUE, GRAYBLUE, LIGHTGREEN, LGRAY, LGRAYBLUE, LBBLUE };
  // const int colormap_length = sizeof(colormap) / sizeof(uint16_t);
  int cycle = 0;

  memset(life, 0, width * height);
  memset(life_line, 0, width);
  memset(life_line_next, 0, width);
  memset(rules, 0, 8);
  life_line[width / 2] = 1;
  //read the rules
  for (int n = 0; n < 8; ++n)
  {
    rules[n] = rule & 1;
    rule >>= 1;
  }

  while (1) {
    for (int n = 0; n < width; ++n)
    {
      uint8_t current_value = 0;

      if (n == 0)
        current_value = (life_line[width-1] << 2) + (life_line[0] << 1) + life_line[1];
      else if (n == width - 1)
        current_value = (life_line[width - 2] << 2) + (life_line[width - 1] << 1) + life_line[0];
      else
        current_value = (life_line[n-1] << 2) + (life_line[n] << 1) + life_line[n + 1];

      life_line_next[n] = rules[current_value];
    }

    memcpy(life, life + width, (height - 1) * width);
    memcpy(life + (height - 1) * width, life_line_next, width);
    memcpy(life_line, life_line_next, width);

    LCD_Address_Set(0, 0, width - 1, height - 1);
    
    for (int n = 0; n < width * height; ++n)
      if(life[n]) {
        // vec3 col = 0.5 + 0.5*cos(iTime+uv.xyx+vec3(0,2,4)); x and y are from 0 to 1
        int x = n % width;
        int y = n / width;
        //uint16_t col = RGB(rangedcos(x, width, 0x20, cycle * 0.2f), rangedcos(x, width, 0x40, 2.0f + cycle * 0.25f), rangedcos(y, height, 0x20, 4.0f + cycle * 0.15f));
        uint16_t col = RGB(rangedcos_u(x + cycle / 2, width * 2, 0x20), rangedcos_u(x + cycle * 4 / 5, width * 2, 0x40), rangedcos_u(y + cycle / 3, height * 2, 0x20));
        LCD_WR_DATA(col);
        //LCD_WR_DATA(colormap[(n + cycle) % (colormap_length - 1) + 1]);
        //LCD_WR_DATA((((n & 0x7FF) + (cycle << 11)) & 0xFFFF) + 1);
      } else {
        LCD_WR_DATA(life[n] ? BLUE : BLACK);
      }
    cycle++;
  }
}

void draw_conway_life() {
  int const width = 160;
  int const height = 80;
  uint8_t life[width * height];
  uint8_t life_next[width * height];
  memset(life, 0, width * height);
  memset(life_next, 0, width * height);
  srand(123413);
  for (int n = 0; n < width * height; ++n)
    life[n] = (rand() % 7) == 1;

  while (1) {
    int idx = 0;
    LCD_Address_Set(0, 0, width - 1, height - 1);      //设置光标位置
    for (int y = 0; y < height; ++y)
      for (int x = 0; x < width; ++x, ++idx) {
        int top = (y == 0) ? height * width - width : -width;
        int bottom = (y == (height - 1)) ? width - height * width : width;
        int left = (x == 0) ? width - 1 : -1;
        int right = (x == (width - 1)) ? 1 - width : 1;
        int n_neighbours = life[idx + top] +
        life[idx + bottom] +
          life[idx + right] +
          life[idx + left] +
          life[idx + top + right] +
          life[idx + top + left] +
          life[idx + bottom + right] +
          life[idx + bottom + left];
        if (life[idx] && (n_neighbours == 2 || n_neighbours == 3)) {
          life_next[idx] = 1;
          LCD_WR_DATA(RED);
          continue;
        }
        if (!life[idx] && n_neighbours == 3) {
          life_next[idx] = 1;
          LCD_WR_DATA(WHITE);
          continue;
        }
        // if (n_neighbours == 1) {
        //   life_next[idx] = 1;
        //   LCD_WR_DATA(RED);
        //   continue;
        // }
        life_next[idx] = 0;
        LCD_WR_DATA(BLACK);
      }

    idx = 0;
    memcpy(life, life_next, width * height);
  	// for (int i = 0; i < width * height; i++)
  		// LCD_WR_DATA(life[i] ? RED : BLACK);
    // delay_1ms(40);
  }


}

int main(void)
{

    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOC);
    gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13);
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1|GPIO_PIN_2);

    init_uart0();

    Lcd_Init();			// init OLED
    LCD_Clear(BLACK);

    // while(TRUE) {
      // draw_angle();
      //draw_mandel();
      //draw_conway_life();
      //draw_1d_ca(110);
      draw_1d_ca(150);
    //}
}

int _put_char(int ch)
{
    usart_data_transmit(USART0, (uint8_t) ch );
    while ( usart_flag_get(USART0, USART_FLAG_TBE)== RESET){
    }

    return ch;
}
