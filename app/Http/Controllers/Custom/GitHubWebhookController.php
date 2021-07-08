<?php

namespace App\Http\Controllers\Custom;

use App\Http\Controllers\Controller;
use Illuminate\Http\Request;
use Illuminate\Support\Facades\Log;

class GitHubWebhookController extends Controller
{
    public function index(Request $request)
    {
        $payload = json_decode($request->payload);
        $branchRef = $payload->ref;
        if ($branchRef !== 'refs/heads/release') {
            return $branchRef;
        }

        exec('git pull 2>&1', $output, $result_code);

        Log::info(sprintf('GitHubWebhook[%d]', $result_code), $output);

        return response($result_code, ($result_code === 0) ? 200 : 500);
    }
}
