<?php

namespace App\Customize;

use Illuminate\Support\Facades\Log;

class TelegramBot
{
    const BOT_ID = '1641207086:AAGhCRobFOkEFrkiGD95CZbWSEV6psliGfQ';

    const API_BOT_URL = 'https://api.telegram.org/bot' . self::BOT_ID;

    /**
     * [Command] sendMessage
     *
     * @param int $chat_id APIURL/getUpdates
     * @param string $message
     *
     * @return {bool} Status
     */
    public function sendMessage($chat_id, $message)
    {
        try {
            $client = new \GuzzleHttp\Client(['timeout' => 20.0]);
            $response = $client->post(
                self::API_BOT_URL . '/sendMessage',
                [
                    'query' => [
                        'chat_id' => $chat_id,
                        'text' => $message,
                    ],
                ]
            );
        } catch(\Throwable $e) {
            Log::error($e);
            return false;
        }

        return true;
    }
}

