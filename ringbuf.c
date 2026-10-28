#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define BUFFER_SIZE 8U
#define BUFFER_MASK (BUFFER_SIZE - 1U)

typedef struct
{
    uint8_t data[BUFFER_SIZE];
    uint16_t head;
    uint16_t tail;
    uint16_t count;
} RingBuffer;

static void ringbuf_init(RingBuffer *buffer)
{
    memset(buffer, 0, sizeof(*buffer));
}

static int ringbuf_write(RingBuffer *buffer, uint8_t value)
{
    if (buffer->count == BUFFER_SIZE)
    {
        return 0;
    }

    buffer->data[buffer->head] = value;

    /* Bitwise AND avoids division/modulo on MCUs without a hardware divider.
       This wrap-around method works only when BUFFER_SIZE is a power of 2. */
    buffer->head = (uint16_t)((buffer->head + 1U) & BUFFER_MASK);
    buffer->count++;

    return 1;
}

static int ringbuf_read(RingBuffer *buffer, uint8_t *value)
{
    if (buffer->count == 0U)
    {
        return 0;
    }

    *value = buffer->data[buffer->tail];
    buffer->tail = (uint16_t)((buffer->tail + 1U) & BUFFER_MASK);
    buffer->count--;

    return 1;
}

static uint16_t ringbuf_count(const RingBuffer *buffer)
{
    return buffer->count;
}

static int ringbuf_is_full(const RingBuffer *buffer)
{
    return buffer->count == BUFFER_SIZE;
}

static int ringbuf_is_empty(const RingBuffer *buffer)
{
    return buffer->count == 0U;
}

int main(void)
{
    RingBuffer buffer;
    uint8_t value;
    uint8_t initial_data[] = {
        0x41U, 0x42U, 0x43U, 0x44U,
        0x45U, 0x46U, 0x47U, 0x48U
    };
    uint8_t new_data[] = {0x49U, 0x4AU, 0x4BU};
    uint16_t i;

    ringbuf_init(&buffer);
    printf("Initial state: count=%u, empty=%s, full=%s\n",
           (unsigned int)ringbuf_count(&buffer),
           ringbuf_is_empty(&buffer) ? "YES" : "NO",
           ringbuf_is_full(&buffer) ? "YES" : "NO");

    for (i = 0U; i < (uint16_t)(sizeof(initial_data) / sizeof(initial_data[0])); i++)
    {
        if (ringbuf_write(&buffer, initial_data[i]))
        {
            printf("[WRITE] 0x%02X -> OK (count=%u)%s\n",
                   (unsigned int)initial_data[i],
                   (unsigned int)ringbuf_count(&buffer),
                   ringbuf_is_full(&buffer) ? " FULL" : "");
        }
        else
        {
            printf("[WRITE] 0x%02X -> FAIL (buffer full)\n",
                   (unsigned int)initial_data[i]);
        }
    }

    if (!ringbuf_write(&buffer, 0x99U))
    {
        printf("[WRITE] 0x99 -> FAIL (buffer full)\n");
    }

    for (i = 0U; i < 3U; i++)
    {
        if (ringbuf_read(&buffer, &value))
        {
            printf("[READ]    -> 0x%02X (count=%u)\n",
                   (unsigned int)value,
                   (unsigned int)ringbuf_count(&buffer));
        }
    }

    for (i = 0U; i < (uint16_t)(sizeof(new_data) / sizeof(new_data[0])); i++)
    {
        if (ringbuf_write(&buffer, new_data[i]))
        {
            printf("[WRITE] 0x%02X -> OK (count=%u)%s\n",
                   (unsigned int)new_data[i],
                   (unsigned int)ringbuf_count(&buffer),
                   ringbuf_is_full(&buffer) ? " FULL" : "");
        }
        else
        {
            printf("[WRITE] 0x%02X -> FAIL (buffer full)\n",
                   (unsigned int)new_data[i]);
        }
    }

    while (ringbuf_read(&buffer, &value))
    {
        printf("[READ]    -> 0x%02X (count=%u)\n",
               (unsigned int)value,
               (unsigned int)ringbuf_count(&buffer));
    }

    printf("Final state: count=%u, empty=%s, full=%s\n",
           (unsigned int)ringbuf_count(&buffer),
           ringbuf_is_empty(&buffer) ? "YES" : "NO",
           ringbuf_is_full(&buffer) ? "YES" : "NO");

    if (!ringbuf_read(&buffer, &value))
    {
        printf("[READ] (empty) -> FAIL (buffer empty)\n");
    }

    return 0;
}
