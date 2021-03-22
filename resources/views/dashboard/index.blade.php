@extends('layouts.app')
@section('title', 'Dashboard | ' . config('app.name'))
@push('head.css')
    <link href="{{ mix('/css/dashboard/app.css') }}" rel="stylesheet">
@endpush
@section('body.content')
    <div id="app"></div>
@endsection
@push('body.js')
    <script src="{{ mix('/js/dashboard/app.js') }}"></script>
@endpush
