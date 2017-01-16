'use strict';
/* eslint-disable quotes */

module.exports = [
    {
        "type": "heading",
        "id": "main-heading",
        "defaultValue": "Personalize Configuration",
        "size": 1
    },

    {
        "type": "text",
        "defaultValue": "Use this configuration page to Personalize your watch!"
    },

    {
        "type": "section",
        "capabilities": ["COLOR"],
        "items": [
            {
                "type": "heading",
                "defaultValue": "COLOR SELECTION"
            },

            {
                "type": "color",
                "messageKey": "BG_COLOR_KEY",
                "defaultValue": "0000FF",
                "label": "Local Time Background Color",
                "sunlight": false
            },

            {
                "type": "color",
                "messageKey": "TEXT_COLOR_KEY",
                "defaultValue": "FFFFFF",
                "label": "Local Time Text Color",
                "sunlight": false
            }
        ]
    },

         //****************
    {
        "type": "section",
        "items": [
            {
                "type": "heading",
                "defaultValue": "DATE FORMAT:"
            },
            {
                "type": "radiogroup",
                "messageKey": "DATE_FORMAT_KEY",
                "label": "",
                "defaultValue": "0",
                "options": [
                    { "label": "Jun 5, 2016", "value": "0" },
                    { "label": "5 Jun 2016",  "value": "1" }
                    
                ]
            },
       //**********************

            {
                "type": "section",
                "items": [
                    {
                        "type": "heading",
                        "defaultValue": "BLUETOOTH:"
                    },
                    {
                        "type": "toggle",
                        "messageKey": "BT_VIBRATE_KEY",
                        "label": "Vibrate on Loss of Bluetooth",
                        "defaultValue": false
                    }
                ]
            },
         
        //******************
            {
                "type": "section",
                "items": [
                    {
                        "type": "heading",
                        "defaultValue": "LOW BATTERY"
                    },
                    {
                        "type": "toggle",
                        "messageKey": "LOW_BATTERY_KEY",
                        "label": "Vibrate on Low Battery",
                        "defaultValue": false
                    }
                ]
            },
       
            //****************************
            {
                "type": "section",
                "items": [
                    {
                        "type": "heading",
                        "defaultValue": ""
                    },
                    {
                        "type": "input",
                        "messageKey": "PERSONALIZED_TEXT_KEY",
                        "defaultValue": "Enter Text",
                        "label": "Personalized Text (11 Characters Maximum)",
                        "description": "",
                        "attributes": {
                            "placeholder": "Enter Personalized Text",
                             "maxlength": 11,

                        }
                    }
                ]
            },
           
            // * * * 
            
            {
                "type": "heading",
                "defaultValue": ""
            },
            {
                "type": "submit",
                "defaultValue": "Save"
            },
            {
                "type": "text",
                "defaultValue": "Personalize By WA1OUI V5.2"
            }
        ]
    }
];