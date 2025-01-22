#include QMK_KEYBOARD_H

// Tap Dance keycodes
enum td_keycodes {
    MSE_BTN2_PRECISION_L5,
    MSE_BTN3_LAYR_CLEAR,

    // Hold mse btn4 toggles dragscroll
    MSE_BTN4_DRAG_L3,
    // held: switch to layer 1,  tap: forward btn
    MSE_BTN5_LAYR_1,  
};

// Define a state for when we're holding down button 4
// this enters precison mode but also allows us to switch to another layer
bool btn4_held = false;
bool btn5_held = false;
bool precision_mode = false;

// Define a type containing as many tapdance states as you need
// Define a type containing as many tapdance states as you need
typedef enum {
    TD_NONE,
    TD_UNKNOWN,
    TD_SINGLE_TAP,
    TD_SINGLE_HOLD,
    TD_DOUBLE_SINGLE_TAP,
    TD_DOUBLE_HOLD,
    TD_DOUBLE_TAP
} td_state_t;

// Create a global instance of the tapdance state type
static td_state_t td_state; 
// Declare your tapdance functions:

// Function to determine the current tapdance state
td_state_t cur_dance(tap_dance_state_t *state);

// `finished` and `reset` functions for each tapdance keycode
void mseBtn4_finished(tap_dance_state_t *state, void *user_data);
void mseBtn4_reset(tap_dance_state_t *state, void *user_data);
void mseBtn5_finished(tap_dance_state_t *state, void *user_data);
void mseBtn5_reset(tap_dance_state_t *state, void *user_data);
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT( /* Base */
        KC_BTN1, TD(MSE_BTN3_LAYR_CLEAR), TD(MSE_BTN2_PRECISION_L5), TD(MSE_BTN4_DRAG_L3), TD(MSE_BTN5_LAYR_1) 
    ),
    [1] = LAYOUT(
        TG(4), DRAG_SCROLL, DPI_CONFIG, LCTL(LSFT(KC_TAB)), _______
    ),
    [2] = LAYOUT( // swap right click
        KC_BTN1, TD(MSE_BTN3_LAYR_CLEAR), TD(MSE_BTN4_DRAG_L3), TD(MSE_BTN2_PRECISION_L5), TD(MSE_BTN5_LAYR_1)
    ),
    [3] = LAYOUT( // alt tab, win tab
        LGUI(KC_TAB), KC_BTN3, LALT(KC_TAB), _______, LCTL(KC_TAB)
    ),
    [4] = LAYOUT( // copy paste layer 
        KC_BTN1, TD(MSE_BTN3_LAYR_CLEAR), LCTL(KC_X), LCTL(KC_C), LCTL(KC_V)
    ),
    [5] = LAYOUT(
        KC_WWW_REFRESH, LSFT(KC_6), _______, LCTL(KC_C), LCTL(KC_V)
    ),
    [6] = LAYOUT(
        KC_BTN1, _______, KC_BTN2, LSFT(KC_TAB), KC_TAB
    ),
};


// Determine the tapdance state to return
td_state_t cur_dance(tap_dance_state_t *state) {
    if (state->count == 1) {
        // Interrupted means some other button was pressed in the tapping term
        if (state->interrupted || !state->pressed) {
            xprintf("TD_SINGLE_TAP\n");
            return TD_SINGLE_TAP;
        } else {
            xprintf("TD_SINGLE_HOLD\n");
            return TD_SINGLE_HOLD;
        }
    }
    if (state->count == 2) {
        if (state->interrupted) {
            xprintf("TD_DOUBLE_SINGLE_TAP\n");
            return TD_DOUBLE_SINGLE_TAP;
        } else if (state->pressed) {
            xprintf("TD_DOUBLE_HOLD\n");
            return TD_DOUBLE_HOLD;
        } else {
            xprintf("TD_DOUBLE_TAP\n");
            return TD_DOUBLE_TAP;
        }
    } else {
        xprintf("TD_UNKNOWN\n");
        return TD_UNKNOWN; // Any number higher than the maximum state value you return above
    }
}

void mseBtn2_finished(tap_dance_state_t *state, void *user_data) {
    td_state = cur_dance(state);
    switch (td_state) {
        case TD_SINGLE_HOLD:
            layer_on(5);
        break;
        case TD_DOUBLE_TAP:
            if (!precision_mode) {
                pmw33xx_set_cpi(0, (dpi_array[keyboard_config.dpi_config] / 2) );
                precision_mode = true;
            } else {
                pmw33xx_set_cpi(0, dpi_array[keyboard_config.dpi_config] );
                precision_mode = false;
            }
        break;
        default:
        break;
    }
}
void mseBtn2_reset(tap_dance_state_t *state, void *user_data) {
    switch (td_state) {
        case TD_SINGLE_TAP:
            tap_code16(KC_BTN2);
        break;
        case TD_SINGLE_HOLD:
            layer_off(5);
        break;
        case TD_DOUBLE_HOLD:
            tap_code16(KC_ENTER);
        break;
        default:
        break;
    }
}

void mseBtn3_finished(tap_dance_state_t *state, void *user_data) {
    td_state = cur_dance(state);
    switch (td_state) {
        case TD_SINGLE_HOLD:
            layer_on(6);
            register_code(KC_LEFT_ALT);
        break;
        default:
        break;
    }
}
void mseBtn3_reset(tap_dance_state_t *state, void *user_data) {
    switch (td_state) {
        case TD_SINGLE_TAP:
            if(layer_state_is(4)) {
                layer_clear();
            } else {
                tap_code16(KC_WWW_REFRESH);
            }
        break;
        case TD_SINGLE_HOLD:
            layer_off(6);
            unregister_code(KC_LEFT_ALT);
        break;
        case TD_DOUBLE_TAP:
            register_code(KC_LCTL);
            tap_code(KC_W);
            unregister_code(KC_LCTL);
        break;
        case TD_DOUBLE_HOLD:
            register_code(KC_LCTL);
            register_code(KC_LSFT);
            tap_code(KC_T);
            unregister_code(KC_LSFT);
            unregister_code(KC_LCTL);
        break;
        default:
        break;
    }
}

void mseBtn4_finished(tap_dance_state_t *state, void *user_data) {
    td_state = cur_dance(state);
    switch (td_state) {
        case TD_SINGLE_HOLD:
            xprintf("Hold for button 4 finished\n");
            is_drag_scroll = true;
            btn4_held = true;
            layer_on(3);
        break;
        default:
        break;
    }
}
void mseBtn4_reset(tap_dance_state_t *state, void *user_data) {
    switch (td_state) {
        case TD_SINGLE_TAP: // single tap always on reset
            xprintf("reset button 4 sending tap code\n");
            tap_code16(KC_BTN4);
        break;
        case TD_SINGLE_HOLD:
            xprintf("Hold for button 4 reset\n");
            is_drag_scroll = false;
            btn4_held = false;
            layer_off(3);
        break;
        case TD_DOUBLE_TAP:
            tap_code16(KC_LEFT);
        break;
        case TD_DOUBLE_HOLD:
            tap_code16(KC_HOME);
            // register_code(KC_LCTL);
            // register_code(KC_LSFT);
            // tap_code(KC_TAB);
            // unregister_code(KC_LSFT);
            // unregister_code(KC_LCTL);
        break;
        default:
        break;
    }
}

// Handle the possible states for each tapdance keycode you define:
void mseBtn5_finished(tap_dance_state_t *state, void *user_data) {
    td_state = cur_dance(state);
    switch (td_state) {
        case TD_SINGLE_HOLD:
            btn5_held = true;
            layer_on(1);
        break;
        default:
        break;
    }
}
void mseBtn5_reset(tap_dance_state_t *state, void *user_data) {
    if(btn4_held) { // antz: no longer use
        switch (td_state) {
            case TD_DOUBLE_TAP:
                xprintf(" button 5 double tapped\n");
                // If button 4 is held we're in drag scroll, so come out of that mode
                is_drag_scroll = false;
                uprintf(" button 4 is held\n");
                if (layer_state_is(0))  {
                    uprintf(" layer state was 0 switching to 2\n");
                    layer_on(2);
                }
                else {
                    uprintf(" layer state was 2 switching to 0\n");
                    layer_clear();
                }
            break;
            default:
            break;
        }
    } else {
        switch (td_state) {
            case TD_SINGLE_TAP:
                tap_code16(KC_BTN5);
            break;
            case TD_SINGLE_HOLD:
                btn5_held = false;
                layer_off(1);
            break;
            case TD_DOUBLE_TAP:
                tap_code16(KC_RIGHT);
            break;
            case TD_DOUBLE_HOLD:
                tap_code16(KC_END);
                // register_code(KC_LCTL);
                // tap_code(KC_TAB);
                // unregister_code(KC_LCTL);
            break;
            default:
            break;
        }
    }
}

// Define `ACTION_TAP_DANCE_FN_ADVANCED()` for each tapdance keycode, passing in `finished` and `reset` functions
tap_dance_action_t tap_dance_actions[] = {
    [MSE_BTN2_PRECISION_L5] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, mseBtn2_finished, mseBtn2_reset),
    [MSE_BTN3_LAYR_CLEAR] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, mseBtn3_finished, mseBtn3_reset),
    [MSE_BTN4_DRAG_L3] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, mseBtn4_finished, mseBtn4_reset),
    [MSE_BTN5_LAYR_1] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, mseBtn5_finished, mseBtn5_reset),
};