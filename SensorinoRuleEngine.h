/** Basic Rule Engine for the sensorino services */
#include <SensorinoService.h>

/*
 * Once we have a more advanced Rule Engine in place we can split this
 * into an abstract RuleEngine class inheriting from SensorinoService, and
 * an actual SimpleRuleEngine implementation class.
 */

class SensorinoRuleEngine: public SensorinoService {
public:
    SensorinoRuleEngine();

    ~SensorinoRuleEngine();

    void handleMessage(boolean broadcast, MessageType msgType,
            DataFormat format, byte *data, int len);

    void handleServiceMessage(byte *data, uint8_t len);

    boolean init(void);
    boolean run(void);

private:
    enum cmd_e {
        CMD_ADD = 0,
        CMD_DEL = 1,
    };

    static const unsigned int serviceNumber;
};
