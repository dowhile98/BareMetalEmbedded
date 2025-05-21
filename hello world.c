#include <stdint.h>

/* 1. Definición de direcciones base (ejemplo STM32F4) */
#define PERIPH_BASE     0x40000000UL                             /**< Base de periféricos */ 
#define AHB1_OFFSET     0x00020000UL                             /**< Offset bus AHB1 */
#define GPIOA_BASE      (PERIPH_BASE + AHB1_OFFSET)              /**< Base de GPIOA: 0x40020000 */
#define RCC_BASE        (PERIPH_BASE + 0x00023800UL)             /**< Base de RCC:   0x40023800 */
#define RCC_AHB1ENR     (*(volatile uint32_t*)(RCC_BASE + 0x30)) /**< RCC AHB1 clock enable register */ 
#define GPIOA_MODER     (*(volatile uint32_t*)(GPIOA_BASE + 0x00)) /**< GPIOA mode register */ 
#define GPIOA_ODR       (*(volatile uint32_t*)(GPIOA_BASE + 0x14)) /**< GPIOA output data register */ 

/* 2. Máscaras de bits */
#define GPIOAEN_BIT     (1U << 0)   /**< Bit0 en RCC_AHB1ENR habilita clock de GPIOA */ 
#define PA5_BIT         (1U << 5)   /**< Bit5 corresponde al pin PA5 */ 

int main(void) {
    /* --- Habilitar reloj de GPIOA --- */
    RCC_AHB1ENR |= GPIOAEN_BIT;    /* OR para poner a 1 el bit GPIOAEN (enable) :contentReference[oaicite:0]{index=0} */

    /* --- Configurar PA5 como salida --- */
    /* Limpiar bits [11:10] de MODER para PA5, luego poner 01 en esos bits */
    GPIOA_MODER &= ~(0x3U << (5 * 2)); /* AND con NOT: borra el par de bits (10–11) :contentReference[oaicite:1]{index=1} */
    GPIOA_MODER |=  (0x1U << (5 * 2)); /* OR para establecer ‘01’ (salida) en MODER5 :contentReference[oaicite:2]{index=2} */

    /* --- Operaciones de salida en PA5 --- */
    /* Encender LED: poner bit5 a 1 */
    GPIOA_ODR |= PA5_BIT;            /* OR para set bit :contentReference[oaicite:3]{index=3} */

    /* … más operaciones … */

    /* Alternar LED (toggle) */
    GPIOA_ODR ^= PA5_BIT;            /* XOR para invertir el bit :contentReference[oaicite:4]{index=4} */

    /* Apagar LED: poner bit5 a 0 */
    GPIOA_ODR &= ~PA5_BIT;           /* AND con NOT: clear bit :contentReference[oaicite:5]{index=5} */

    /* Leer estado de PA5 */
    uint32_t estado = (GPIOA_ODR & PA5_BIT) ? 1 : 0; /* AND para enmascarar y comprobar :contentReference[oaicite:6]{index=6} */

    while (1) {
        /* Bucle infinito */
    }
    return 0;
}
	