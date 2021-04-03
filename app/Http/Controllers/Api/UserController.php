<?php

namespace App\Http\Controllers\Api;

use App\Exceptions\ApiErrorException;
use App\Http\Controllers\Controller;
use App\Models\User;
use Illuminate\Http\Request;
use Illuminate\Http\Resources\Json\JsonResource;
use Illuminate\Support\Facades\Auth;
use Illuminate\Support\Facades\DB;
use Illuminate\Support\Facades\Hash;
use Illuminate\Support\Facades\Log;

class UserController extends Controller
{
    /**
     * Display a listing of the resource.
     *
     * @return \Illuminate\Http\Response
     */
    public function index(Request $request)
    {
        $userQuery = new User;

        $authUser = $request->user();

        if (!$authUser->hasPermission(User::ROLE_ADMIN_MASTER)) {
            $userQuery = $userQuery->where('role', '<', User::ROLE_ADMIN_MANAGER);
        }

        foreach (['name', 'phone', 'email', 'sns_info'] as $value) {
            if (isset($request->{$value})) {
                $userQuery = $userQuery->where($value, 'LIKE', '%'.(is_array($request->{$value}) ? $request->{$value}[0] : $request->{$value}).'%');
            }
        }

        return new JsonResource( $userQuery->paginate());
    }

    /**
     * Store a newly created resource in storage.
     *
     * @param  \Illuminate\Http\Request  $request
     * @return \Illuminate\Http\Response
     */
    public function store(Request $request)
    {
        try {
            DB::beginTransaction();

            $authUser = $request->user();

            if (!$authUser->hasPermission(User::ROLE_ADMIN_MANAGER)) {
                throw new ApiErrorException('Bạn không có quyền chỉnh sửa người dùng này');
            }

            $data = $request->toArray();
            $data['password'] = Hash::make($data['password'] ?? rand());
            $data['email'] = $data['email'] ?? time() . '@rinphone.vn';
            $data['role'] = $authUser->hasPermission(User::ROLE_ADMIN_MASTER) ? $data['role'] : User::ROLE_USER_NORMAL;

            $user = new User;
            $user->fill($data);
            $user->save();

            DB::commit();
        } catch(\Throwable $e) {
            DB::rollback();

            if ($e instanceof ApiErrorException) {
                return response([
                    'message' => $e->getMessage()
                ], 400);
            }

            Log::error($e);

            return response(null, 500);
        }

        return $this->show($user);
    }

    /**
     * Display the specified resource.
     *
     * @param  int  $id
     * @return \Illuminate\Http\Response
     */
    public function show(User $user)
    {
        return new JsonResource($this->canModify($user) ? $user : null);
    }

    /**
     * Update the specified resource in storage.
     *
     * @param  \Illuminate\Http\Request  $request
     * @param  User  $user
     * @return \Illuminate\Http\Response
     */
    public function update(Request $request, User $user)
    {
        try {
            DB::beginTransaction();

            if (!$this->canModify($user)) {
                throw new ApiErrorException('Bạn không có quyền chỉnh sửa người dùng này');
            }

            $authUser = $request->user();

            $data = $request->toArray();
            $data['role'] = $authUser->hasPermission(User::ROLE_ADMIN_MASTER) ? $data['role'] : User::ROLE_USER_NORMAL;

            $user->fill($data);
            $user->save();

            DB::commit();
        } catch(\Throwable $e) {
            DB::rollback();

            if ($e instanceof ApiErrorException) {
                return response([
                    'message' => $e->getMessage()
                ], 400);
            }

            Log::error($e);

            return response(null, 500);
        }

        return $this->show($user);
    }

    /**
     * Remove the specified resource from storage.
     *
     * @param  int  $id
     * @return \Illuminate\Http\Response
     */
    public function destroy($id)
    {
        //
    }

    private function canModify(User $user)
    {
        $authUser = Auth::user();
        return ($authUser->hasPermission(User::ROLE_ADMIN_MASTER)
                || ($user && ($authUser->role > $user->role)));
    }
}
