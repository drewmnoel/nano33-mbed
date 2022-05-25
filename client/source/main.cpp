#include <events/mbed_events.h>
#include <mbed.h>
#include "ble/BLE.h"
#include "ble/Gap.h"
#include "gap/AdvertisingDataParser.h"
#include "USBSerial.h"
#include "BatteryService.h"
extern USBSerial serial;

using namespace ble;
using namespace mbed;

static const char DEVICE_NAME[] = "LE Beacon";

class DualPHYScan : private mbed::NonCopyable<DualPHYScan>, public ble::Gap::EventHandler
{
public:
    static const uint16_t MAX_ADVERTISING_PAYLOAD_SIZE = 59;

    DualPHYScan(BLE& ble, events::EventQueue& event_queue) :
        _ble(ble),
        _gap(ble.gap()),
        _event_queue(event_queue),
        _led1(A7, 0),
        _battery_uuid(GattService::UUID_BATTERY_SERVICE),
        _battery_level(100),
        _battery_service(ble, _battery_level),
        _adv_data_builder(_adv_buffer),
        _adv_handle(ble::INVALID_ADVERTISING_HANDLE)
    {
    }

    ~DualPHYScan()
    {
        if (_ble.hasInitialized()) {
            _ble.shutdown();
        }
    }

    void run();

private:
    /** This is called when BLE interface is initialised and starts the first mode */
    void on_init_complete(BLE::InitializationCompleteCallbackContext *event);

    void startScanForever(void);

    /** Set up and start scanning coded phy*/
    void scan_coded_phy(void);

    void update_sensor_value(void);

    /** Blink LED to show we're running */
    void blink(void);

private: //following are virtual functions

    /* Gap::EventHandler */

    /** Look at scan payload and parse the packet **/
    virtual void onAdvertisingReport(
    const ble::AdvertisingReportEvent &event
    ) {
        
        ble::AdvertisingDataParser adv_parser(event.getPayload());

        /* parse the advertising payload, looking for a discoverable device */
        while (adv_parser.hasNext()) {
            ble::AdvertisingDataParser::element_t field = adv_parser.next();

            if (field.type == ble::adv_data_type_t::COMPLETE_LOCAL_NAME &&
                field.value.size() == strlen(DEVICE_NAME) &&
                (memcmp(field.value.data(), DEVICE_NAME, field.value.size()) == 0)) {
                    printf("Peer device: %d\r\n", event.getRssi());
		    _led1 = 1;
            }
        }
    }

    virtual void onScanTimeout(
        const ble::ScanTimeoutEvent&
    ) {
    }

    virtual void onConnectionComplete(
        const ble::ConnectionCompleteEvent &event
    ) {
    }

    virtual void onDisconnectionComplete(
        const ble::DisconnectionCompleteEvent &event
    ) {
    }

private:
    BLE                &_ble;
    ble::Gap           &_gap;
    events::EventQueue &_event_queue;

    DigitalOut _led1;

    UUID            _battery_uuid;
    uint8_t         _battery_level;
    BatteryService  _battery_service;
    uint8_t _adv_buffer[MAX_ADVERTISING_PAYLOAD_SIZE];
    ble::AdvertisingDataBuilder _adv_data_builder;

    ble::advertising_handle_t _adv_handle;
};

void DualPHYScan::run(void) {
    if (_ble.hasInitialized()) {
        printf("Ble instance already initialised.\r\n");
        return;
    }

    _gap.setEventHandler(this);

    ble_error_t error = _ble.init(this, &DualPHYScan::on_init_complete);
    if (error) {
        return;
    }

    _event_queue.call_every(1000, this, &DualPHYScan::blink);
    _event_queue.dispatch_forever();
}

void DualPHYScan::on_init_complete(BLE::InitializationCompleteCallbackContext *event) {
    if (event->error) {
        return;
    }

    if (!_gap.isFeatureSupported(ble::controller_supported_features_t::LE_EXTENDED_ADVERTISING) ||
            !_gap.isFeatureSupported(ble::controller_supported_features_t::LE_PERIODIC_ADVERTISING)) {
        printf("Periodic advertising not supported, cannot run example.\r\n");
        return;
    }

    _event_queue.call_in(10, this, &DualPHYScan::scan_coded_phy);
}

void DualPHYScan::scan_coded_phy(void) {
    ble::ScanParameters scan_params(
            ble::phy_t::LE_CODED,
            ble::scan_interval_t::min(),
            ble::scan_window_t(80),
            true,
            ble::own_address_type_t::RANDOM,
            ble::scanning_filter_policy_t::NO_FILTER);

    ble_error_t error = _gap.setScanParameters(scan_params);

    if (error) {
        return;
    }

    error = _gap.startScan(ble::scan_duration_t(0));

    if (error) {
        return;
    }
}

void DualPHYScan::blink(void) {
    _led1 = 0;
}

events::EventQueue event_queue;

void schedule_ble_events(BLE::OnEventsToProcessCallbackContext *context) {
    event_queue.call(Callback<void()>(&context->ble, &BLE::processEvents));
}

int main() {
    serial.connect();
    serial.wait_ready();
    BLE &ble = BLE::Instance();
    ble.onEventsToProcess(schedule_ble_events);
    DualPHYScan DualPhyScan(ble, event_queue);
    DualPhyScan.run();
    return 0;
}
