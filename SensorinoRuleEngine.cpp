/** Basic Rule Engine for the sensorino services */
#include <Sensorino.h>
#include <limits.h>

const unsigned int SensorinoRuleEngine::serviceNumber = -1;

SensorinoRuleEngine::SensorinoRuleEngine(void) :
        SensorinoService(serviceNumber, 0, INT_MAX, 0) {
    if (!addRuleEngine(this))
        /* throw new Exception("Can't addRuleEngine"); */
        ;
}

SensorinoRuleEngine::~SensorinoRuleEngine(void) {
    /* TODO: remove the service */
}

/* Simplify generated code by making these global instead of class variables */
static uint8_t rule_cnt;
#define MAX_RULES 8
static struct sensorino_rule_s {
    uint8_t if_len, then_len;
    uint8_t if_data[16], then_data[16];
} ruleset[MAX_RULES];
boolean rule_running; /* Simple guard against nesting */

void SensorinoRuleEngine::handleServiceMessage(byte *data, uint8_t len) {
    struct sensorino_rule_s *rule;

    if (rule_running)
        return;
    rule_running = true;

    uint8_t i;
    for (i = rule_cnt, rule = ruleset; i; i--, rule++) {
        MessageType msgType;

        /* Check if the rule's "if" part matches {data, len} */
        if (rule->if_len != len)
            continue;

        uint8_t j;
        for (j = 0; j < len; j++)
            if (rule->if_data[j] != data[j])
                break;
        if (j < len)
            continue;

        /* It does! */
        msgType = (MessageType) rule->then_data[0];
        ::handleMessage(false, msgType,
                rule->then_data + 1, rule->then_len - 1);
    }

    rule_running = false;
}

void SensorinoRuleEngine::handleMessage(boolean broadcast, MessageType msgType,
        DataFormat format, byte *data, int len) {
    uint8_t cmd, flags, i;
    uint8_t if_len, then_len;

    if (msgType != SET)
        goto err;
    if (format != ADHOC)
        goto err;
    if (len < 2) /* Check if header present */
        goto err;

    /* Parse the header */
    cmd = data[0] >> 4;
    flags = data[0] & 15;
    if_len = data[1] >> 4;
    then_len = data[1] & 15;
    if (len < 2 + if_len + then_len)
        goto err;

    switch (cmd) {
    case CMD_ADD: {
        if (rule_cnt >= MAX_RULES - 1)
            goto err;

        /* Add the new rule and copy the values */
        struct sensorino_rule_s &rule = ruleset[rule_cnt++];
        rule.if_len = if_len;
        rule.then_len = then_len;

        for (i = 0; i < if_len; i++)
            rule.if_data[i] = data[2 + i];
        for (i = 0; i < then_len; i++)
            rule.then_data[i] = data[2 + if_len + i];

        /* Nothing else to do here */
        break;
    }

    case CMD_DEL:
        /* For now this commands just outright purges the ruleset */
        rule_cnt = 0;

        break;

    default:
        goto err;
    }

    return;

err:
    /* TODO */
    return;
}

boolean SensorinoRuleEngine::init(void) {
    return true;
}

boolean SensorinoRuleEngine::run(void) {
    return true;
}
/* vim:set sw=4 ts=4 et: */
